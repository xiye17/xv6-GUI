#include "types.h"
#include "x86.h"
#include "param.h"
#include "defs.h"
#include "msg.h"
#include "xv6_api.h"
#include "spinlock.h"
#include "gui_base.h"
#include "gui_kernal.h"
#include "mmu.h"
#include "proc.h"
//struct wndInfo wndInfoList[MAX_WINDOW_COUNT];
int  focus =  -1;
struct MousePos mousePos = {0, 0};
struct MousePos lastMousePos = {0, 0};
struct WndInfo wndInfoList[MAX_WINDOW_COUNT];
struct TimerInfo timerInfo;
int focusList[MAX_WINDOW_COUNT];
int wndCount = 0;
struct spinlock guiKernelLock;
int mouseDownInContent = 0;
int mouseDownInBar = 0;

int min(int a, int b) {
    return (a > b) ? b : a;
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

int abs(int a) {
    return (a >= 0)? a: -a;
}

int testXXX(RGB * p)
{
    cprintf("%d %d %d", p->R, p->G, p->B);
    cprintf("Test XXX\n");
    return 0;
}

// return 0 not in return 1 in content return 2 in bar
int mouseInWin(int px, int py, int hwnd)
{
    Rect * body = &wndInfoList[hwnd].wndBody;
    Rect * bar = &wndInfoList[hwnd].wndTitleBar;

    if(px <= body->x || px >= (body->x + body->w))
        return 0;

    if(py > body->y && py <(body->y + body->h))
        return 1;
    if(py > bar->y && py < (bar->y + bar->h))
    {
        if(px > body->x && px < body->x + body->w - 30)
            return 2;
        else
            return 3;
    }

    return 0;
}
void  initMsgQueue(MsgQueue * msgQ)
{
    msgQ->head = 0;
    msgQ->tail = 0;
    msgQ->length = 0;
    memset(msgQ->msgList, 0, MAX_MSG_COUNT * sizeof(message));
}

int isQueueEmpty(MsgQueue *msgQ)
{
    if(msgQ->head==msgQ->tail)
        return 1;
    else
        return 0;
}

int isQueueFull(MsgQueue *msgQ)
{
    if(msgQ->head==(msgQ->tail + 1) % MAX_MSG_COUNT)
        return 1;
    else
        return 0;
}

int addMsgToQueue(MsgQueue *msgQ, message *msg)
{
    if(msg->msg_type == M_CLOSE_WINDOW)
    {
         msgQ->msgList[msgQ->tail].msg_type = M_CLOSE_WINDOW;
    }
    if(isQueueFull(msgQ))
        return 0;
    msgQ->msgList[msgQ->tail].msg_type = msg->msg_type;
    memmove(msgQ->msgList[msgQ->tail].params, msg->params, 10 * sizeof(int));
    msgQ->tail = (msgQ->tail + 1) % MAX_MSG_COUNT;
    msgQ->length++;
    return 1;
}

int getMessageFromQueue(MsgQueue *msgQ, message * msg)
{
    if(isQueueEmpty(msgQ))
        return 0;
    msg->msg_type = msgQ->msgList[msgQ->head].msg_type;
    memmove(msg->params, msgQ->msgList[msgQ->head].params, 10 * sizeof(int));
    msgQ->head=(msgQ->head + 1) % MAX_MSG_COUNT;
    msgQ->length--;
    return 1;
}

void
initGUIKernel()
{
    cprintf("guiKernelInit\n");
    int i;
    for(i = 0; i < MAX_WINDOW_COUNT; ++i)
        wndInfoList[i].hwnd = -1;

    timerInfo.ticks = -1;
    for(i = 0; i < MAX_WINDOW_COUNT; ++i)
    {
        timerInfo.intervalList[i] = -1;
        timerInfo.countList[i] = -1;
    }

}

#define MOUSE_SPEED_X 0.6f
#define MOUSE_SPEED_Y -0.6f

int dispatchMessage(int hwnd, message *msg)
{
    if(addMsgToQueue(&wndInfoList[hwnd].msgQ, msg))
        return 1;
    return 0;
}

int drawWndTitleBar(int hwnd)
{
    WndInfo *wnd = &wndInfoList[hwnd];
    if (hwnd==0)
        return 0;
    int i = 0;
    /*int x = wnd->wndTitleBar.x;*/
    /*int y = wnd->wndTitleBar.y;*/
    int w = wnd->wndTitleBar.w;
    int h = wnd->wndTitleBar.h;
    RGB * o;
    RGB * buf = wnd->wholeContent;
    for(i = 0; i < h; ++i)
    {
        o = buf +  i * w;
        memset(o, 118, (w - h) * 3);
        o = buf +  i * w + w - h ;
        memset(o, 0,  h * 3);
    }
    drawStringToContent(buf, 10, 5, w,h,wnd->title, (RGBA){255, 255,255,255});
    return 0;
}

int repaintAllWindow(int hwnd)
{
    int i;
    for (i = 0; i < wndCount; ++i) {
        switchuvm(wndInfoList[focusList[i]].procPtr);
       // drawWndTitleBar(screen_buf2, i);
        drawRGBContentToContent(screen_buf2, wndInfoList[focusList[i]].wholeContent, wndInfoList[focusList[i]].wndBody.x,
                wndInfoList[focusList[i]].wndTitleBar.y, wndInfoList[focusList[i]].wndBody.w, wndInfoList[focusList[i]].wndBody.h + 30);
        if (i == wndCount - 2) {
            // drawWndTitleBar(screen_buf1, i);
            drawScreenToScreen(screen_buf1, screen_buf2);
        }
        if (proc == 0) {
            switchkvm();
        } else {
            switchuvm(proc);
        }
    }
    drawScreenToScreen(screen, screen_buf2);
    drawMouse(screen, 0, mousePos.x, mousePos.y);
    return 0;
}

int focusOnWindow(int hwnd)
{
    focus = hwnd;
    if (wndCount >= 1 && hwnd == focusList[0]) {
        repaintAllWindow(hwnd);
        return 0;
    }
    int i;
    for (i = 0; i < wndCount; ++i)
    {
        if (focusList[i] == hwnd)
        {
            break;
        }
    }

    int j;
    for (j = i; j < wndCount - 1; ++j)
    {
        focusList[j] = focusList[j + 1];
    }
    focusList[wndCount - 1] = hwnd;

    repaintAllWindow(hwnd);
    return 0;
}

void
guiKernelHandleMsg(message *msg)
{
    acquire(&guiKernelLock);
    message tempMsg;
    int i;
    int tempR;
    switch(msg->msg_type)
    {
    case M_MOUSE_MOVE:
        lastMousePos = mousePos;
        mousePos.x += msg->params[0] * MOUSE_SPEED_X;
        mousePos.y += msg->params[1] * MOUSE_SPEED_Y;
        if (mousePos.x < 0) {
            mousePos.x = 0;
        }
        if (mousePos.x > SCREEN_WIDTH) {
            mousePos.x = SCREEN_WIDTH;
        }
        if (mousePos.y < 0) {
            mousePos.y = 0;
        }
        if (mousePos.y > SCREEN_WIDTH) {
            mousePos.y = SCREEN_WIDTH;
        }
        if (mouseDownInBar) {
            int dx = mousePos.x - lastMousePos.x;
            int dy = mousePos.y - lastMousePos.y;
            WndInfo* wnd = &wndInfoList[focus];
            int nx = wndInfoList[focus].wndTitleBar.x + dx;
            int ny = wndInfoList[focus].wndTitleBar.y + dy;
            drawRGBContentToContentPart(screen_buf2, screen_buf1, wnd->wndTitleBar.x, wnd->wndTitleBar.y,
                                        wnd->wndTitleBar.x, wnd->wndTitleBar.y, SCREEN_HEIGHT, SCREEN_WIDTH,
                                        wnd->wndBody.h + 30, wnd->wndBody.w);
            switchuvm(wndInfoList[focus].procPtr);
            drawRGBContentToContent(screen_buf2, wnd->wholeContent, nx, ny, wnd->wndBody.w, wnd->wndBody.h + 30);
            int bx = min(wnd->wndTitleBar.x, nx);
            int bw = wnd->wndBody.w + abs(dx);
            int by = min(wnd->wndTitleBar.y, ny);
            int bh = wnd->wndBody.h + abs(dy) + 30;
            drawRGBContentToContentPart(screen, screen_buf2, bx, by, bx, by, SCREEN_HEIGHT, SCREEN_WIDTH, bh, bw);
            wnd->wndTitleBar.x += dx;
            wnd->wndTitleBar.y += dy;
            wnd->wndBody.x += dx;
            wnd->wndBody.y += dy;
            if (proc == 0) {
                switchkvm();
            } else {
                switchuvm(proc);
            }
        }
        if (mouseDownInContent) {
            tempMsg.msg_type = M_MOUSE_MOVE;
            tempMsg.params[0] = mousePos.x - wndInfoList[focus].wndBody.x;
            tempMsg.params[1] = mousePos.y - wndInfoList[focus].wndBody.y;
            dispatchMessage(focus, &tempMsg);
        }
        clearMouse(screen, screen_buf2,lastMousePos.x, lastMousePos.y);
        drawMouse(screen, 0, mousePos.x, mousePos.y);
        break;
    case M_MOUSE_DOWN:
        tempR = 0;
        for (i = wndCount - 1; i >= 0; i--) {
            tempR = mouseInWin(mousePos.x, mousePos.y, focusList[i]);
            if(tempR != 0)
                break;
        }
        if(tempR == 1) {
            mouseDownInContent = 1;
        }
        if(tempR == 2) {
            mouseDownInBar = 1;
        }
        if (focus != focusList[i]) {
            focusOnWindow(focusList[i]);
        }
        tempMsg.msg_type = M_MOUSE_DOWN;
        tempMsg.params[0] = mousePos.x - wndInfoList[focus].wndBody.x;
        tempMsg.params[1] = mousePos.y - wndInfoList[focus].wndBody.y;
        dispatchMessage(focus, &tempMsg);
        if (tempR == 3) {
            tempMsg.msg_type = M_CLOSE_WINDOW;
            dispatchMessage(focus, &tempMsg);
        }
        break;
    case M_MOUSE_UP:
        mouseDownInBar = mouseDownInContent = 0;
        break;
    case M_MOUSE_LEFT_CLICK:
        if (mouseDownInContent)
        {
            mouseDownInContent = 0;
            tempMsg.msg_type = msg->msg_type;
            tempMsg.params[0] = mousePos.x - wndInfoList[focus].wndBody.x;
            tempMsg.params[1] = mousePos.y - wndInfoList[focus].wndBody.y;
            tempMsg.params[2] = 0; //define 0 left click

            dispatchMessage(focus, &tempMsg);
        }
        break;
    case M_MOUSE_RIGHT_CLICK:
        if (mouseDownInContent)
        {
            mouseDownInContent = 0;
            tempMsg.msg_type = msg->msg_type;
            tempMsg.params[0] = mousePos.x - wndInfoList[focus].wndBody.x;
            tempMsg.params[1] = mousePos.y - wndInfoList[focus].wndBody.y;
            tempMsg.params[2] = 1; //define 1 left click
            dispatchMessage(focus, &tempMsg);
        }
        break;
    case M_MOUSE_DBCLICK:
        if (mouseDownInContent)
        {
            mouseDownInContent = 0;
            tempMsg.msg_type = msg->msg_type;
            tempMsg.params[0] = mousePos.x - wndInfoList[focus].wndBody.x;
            tempMsg.params[1] = mousePos.y - wndInfoList[focus].wndBody.y;
            tempMsg.params[2] = 2; //define 2 double click
            dispatchMessage(focus, &tempMsg);
        }
        break;
    case M_KEY_DOWN:
        //cprintf("M_KEY_DOWN");
        tempMsg.msg_type = msg->msg_type;
        tempMsg.params[0] = msg->params[0];
        tempMsg.params[1] = msg->params[1];
        dispatchMessage(focus, &tempMsg);
        break;
    case M_KEY_UP:
        //cprintf("M_KEY_UP");
        tempMsg.msg_type = msg->msg_type;
        tempMsg.params[0] = msg->params[0];
        tempMsg.params[1] = msg->params[1];
        dispatchMessage(focus, &tempMsg);
        break;
    case M_TIMER:
        tempMsg.msg_type = msg->msg_type;
        tempMsg.params[0] = msg->params[0];
        for(i = 0; i < wndCount; ++i)
        {
            timerInfo.ticks = msg->params[0];
            if(timerInfo.intervalList[i]==-1)
                continue;
            timerInfo.countList[i] += 10;
            if (timerInfo.countList[i] % timerInfo.intervalList[i] == 0)
                dispatchMessage(i, &tempMsg);
        }
        break;
    }
    release(&guiKernelLock);
}


void setRect(struct Rect *rect, int x, int y, int w, int h)
{
    rect->x = x;
    rect->y = y;
    rect->w = w;
    rect->h = h;
}

void initDesktop() {
    drawRGBContentToContent(screen, wndInfoList[0].content, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    drawRGBContentToContent(screen_buf1, wndInfoList[0].content, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    drawRGBContentToContent(screen_buf2, wndInfoList[0].content, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

int sys_createwindow(void)
{
    int x, y, cx, cy;
    char * title;
    struct RGB * content;
    argint(0, &x);
    argint(1, &y);
    argint(2, &cx);
    argint(3, &cy);
    argstr(4, &title);
    int p;
    argint(5, &p);
    content = (RGB *) p;

    acquire(&guiKernelLock);
    //Add to the wndList

    int i;
    for (i = 0; i < MAX_WINDOW_COUNT; ++i)
    {
        if(wndInfoList[i].hwnd == -1)
        {
            wndInfoList[i].hwnd = i;
            setRect(&wndInfoList[i].wndTitleBar, x, y - 30, cx, 30);
            setRect(&wndInfoList[i].wndBody,x, y, cx, cy);
            wndInfoList[i].procPtr = proc;
            wndInfoList[i].wholeContent = content;
            wndInfoList[i].content = content + 30 * cx;
            wndInfoList[i].title = title;
            drawWndTitleBar(i);
            initMsgQueue(&wndInfoList[i].msgQ);
            wndCount += 1;
            focusOnWindow(i);
            break;
       }
    }
    release(&guiKernelLock);
    return i;
}

int repainted = 0;
int sys_repaintwindow()
{
    if (repainted == 0) {
        initDesktop();
        repainted = 1;
    }
    int hwnd;
    argint(0, &hwnd);
    acquire(&guiKernelLock);
    if (hwnd == focus) {
        switchuvm(wndInfoList[focus].procPtr);
        WndInfo* wnd = &wndInfoList[focus];
        drawRGBContentToContent(screen_buf2, wnd->wholeContent, wnd->wndTitleBar.x, wnd->wndTitleBar.y, wnd->wndBody.w, wnd->wndBody.h + 30);
        drawRGBContentToContent(screen, wnd->wholeContent, wnd->wndTitleBar.x, wnd->wndTitleBar.y, wnd->wndBody.w, wnd->wndBody.h + 30);
        if (proc == 0) {
            switchkvm();
        } else {
            switchuvm(proc);
        }
    } else {
        repaintAllWindow(hwnd);
    }
    release(&guiKernelLock);
   return 0;
}

int sys_settimer()
{
    int hwnd, interval;
    argint(0, &hwnd);
    argint(1, &interval);

    acquire(&guiKernelLock);

    timerInfo.intervalList[hwnd] = interval;
    timerInfo.countList[hwnd] = 0;
    release(&guiKernelLock);
    return 0;
}

int sys_getmessage()
{
    int hwnd, p;
    argint(0, &hwnd);
    argint(1, &p);
    message *msg = (message *) p;

    if (proc != wndInfoList[hwnd].procPtr)
        return 0;
    acquire(&guiKernelLock);
    int r = getMessageFromQueue(&wndInfoList[hwnd].msgQ, msg);
    release(&guiKernelLock);
    return r;
}

int updateWindow(int hwnd, int x, int y, int w, int h)
{
    WndInfo * wnd = &wndInfoList[hwnd];
    int bx = x;
    int by = y;
    int bh = wnd->wndBody.h;
    int bw = wnd->wndBody.w;

    x = x + wnd->wndBody.x;
    y = y + wnd->wndBody.y;
    /*void drawRGBContentToContentPart(RGB *buf, RGB *img, int x, int y,*/
    /*int bx, int by, int bh, int bw, int h, int w)*/

    drawRGBContentToContentPart(screen_buf2, wnd->content, x, y, bx, by, bh, bw, h, w);
    drawRGBContentToContentPart(screen, wnd->content, x, y, bx, by, bh, bw, h, w);

    return 0;
}

int sys_updatewindow()
{
    int hwnd, x, y, cx, cy;
    argint(0, &hwnd);
    argint(1, &x);
    argint(2, &y);
    argint(3, &cx);
    argint(4, &cy);

    acquire(&guiKernelLock);
    updateWindow(hwnd, x, y, cx, cy);

    release(&guiKernelLock);
    return 0;
}

int sys_destroywindow()
{
    int hwnd;
    argint(0, &hwnd);
    acquire(&guiKernelLock);
    wndInfoList[hwnd].hwnd = -1;
    timerInfo.countList[hwnd] = -1;
    wndCount -= 1;
    focusOnWindow(focusList[wndCount - 1]);
    release(&guiKernelLock);
    return 0;
}
