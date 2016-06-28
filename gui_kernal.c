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

int testXXX(RGB * p)
{
    cprintf("%d %d %d", p->R, p->G, p->B);
    cprintf("Test XXX\n");
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
    cprintf("warnning, queue overflow");
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
        switchuvm(wndInfoList[i].procPtr);
       // drawWndTitleBar(screen_buf2, i);
        drawRGBContentToContent(screen_buf2, wndInfoList[i].wholeContent, wndInfoList[i].wndBody.x,
                wndInfoList[i].wndTitleBar.y, wndInfoList[i].wndBody.w, wndInfoList[i].wndBody.h + 30);
        if (i != wndCount - 1) {
           // drawWndTitleBar(screen_buf1, i);
            drawRGBContentToContent(screen_buf1, wndInfoList[i].wholeContent, wndInfoList[i].wndBody.x,
                    wndInfoList[i].wndTitleBar.y, wndInfoList[i].wndBody.w, wndInfoList[i].wndBody.h + 30);
        }
        if (proc == 0) {
            switchkvm();
        } else {
            switchuvm(proc);
        }
    }
    drawRGBContentToContent(screen, screen_buf2, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    return 0;
}

int focusOnWindow(int hwnd)
{
    focus = hwnd;
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
            cprintf("%d", wndCount);
        clearMouse(screen, screen_buf2,lastMousePos.x, lastMousePos.y);
        drawMouse(screen, 0, mousePos.x, mousePos.y);
        break;
    case M_MOUSE_DOWN:
        for (i = wndCount - 1; i >= 0; i--) {
            if (mousePos.x > wndInfoList[i].wndBody.x && mousePos.x < wndInfoList[i].wndBody.x + wndInfoList[i].wndBody.w && mousePos.y > wndInfoList[i].wndBody.y && mousePos.y < wndInfoList[i].wndBody.y + wndInfoList[i].wndBody.h) {
                break;
            }
        }
        cprintf("%d", i);
        if (focus != i) {
            focusOnWindow(i);
        }
        break;
    case M_MOUSE_UP:
        break;
    case M_MOUSE_LEFT_CLICK:
        break;
    case M_MOUSE_RIGHT_CLICK:
        break;
    case M_MOUSE_DBCLICK:
        break;
    case M_KEY_DOWN:
        //cprintf("M_KEY_DOWN");
        tempMsg.msg_type = msg->msg_type;
        tempMsg.params[0] = msg->params[0];
        tempMsg.params[1] = msg->params[1];
        dispatchMessage(0, &tempMsg);
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
    repaintAllWindow(hwnd);
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
