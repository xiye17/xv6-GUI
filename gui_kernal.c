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
int wndCount = 0;
struct spinlock guiKernelLock;

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
    initlock(&guiKernelLock, "guiKernel");
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

void
guiKernelHandleMsg(message *msg)
{
    acquire(&guiKernelLock);
    message tempMsg;
    switch(msg->msg_type)
    {
    case M_MOUSE_MOVE:
        lastMousePos = mousePos;
        mousePos.x += msg->params[0] * MOUSE_SPEED_X;
        mousePos.y += msg->params[1] * MOUSE_SPEED_Y;
        if (mousePos.x < 0) mousePos.x = 0;
        if (mousePos.x > SCREEN_WIDTH) mousePos.x = SCREEN_WIDTH;
        if (mousePos.y < 0) mousePos.y = 0;
        if (mousePos.y > SCREEN_WIDTH) mousePos.y = SCREEN_WIDTH;

        drawMouse(screen, 0, mousePos.x, mousePos.y);
        break;
    case M_MOUSE_DOWN:
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
        dispatchMessage(focus, &tempMsg);
        break;
    case M_KEY_UP:
        //cprintf("M_KEY_UP");
        tempMsg.msg_type = msg->msg_type;
        tempMsg.params[0] = msg->params[0];
        tempMsg.params[1] = msg->params[1];
        dispatchMessage(focus, &tempMsg);
        break;
    }
    release(&guiKernelLock);
}


void setRect(struct Rect *rect, int x, int y, int h, int w)
{
    rect->x = x;
    rect->y = y;
    rect->h = h;
    rect->w = w;
}

int focusOnWindow(int hwnd)
{
    focus = hwnd;
    return 0;
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
            setRect(&wndInfoList[i].wndTitleBar, x, y - 20, cx, 20);
            setRect(&wndInfoList[i].wndBody,x, y, cx, cy);
            wndInfoList[i].procPtr = proc;
            wndInfoList[i].content = content;
            initMsgQueue(&wndInfoList[i].msgQ);
            focusOnWindow(i);
            break;
       }
    }

    //Desktop Init
    if (i == 0)
    {

    }

    release(&guiKernelLock);
    return 0;
}

int testXXX(RGB * p)
{
    cprintf("%d %d %d", p->R, p->G, p->B);
    cprintf("Test XXX\n");
    return 0;
}

int sys_repaintwindow()
{
    int hwnd;
    argint(0, &hwnd);

    RGB* p = wndInfoList[hwnd].content;
    drawRGBContentToContent(screen, p,0, 0, 800, 600);
    //if (proc == 0)
//		switchkvm();
//	else
//		switchuvm(proc);
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
