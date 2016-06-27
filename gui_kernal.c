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

struct Focus focus = {0, 0, 0};
struct MousePos mousePos = {0, 0};
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

void
initGUIKernel()
{
    cprintf("guiKernelInit\n");
    int i;
    for(i = 0; i < MAX_WINDOW_COUNT; ++i)
        wndInfoList[i].hwnd = -1;
    initlock(&guiKernelLock, "guiKernel");
}

void
guiKernelHandleMsg(message *msg)
{

}


void setRect(struct Rect *rect, int x, int y, int h, int w)
{
    rect->x = x;
    rect->y = y;
    rect->h = h;
    rect->w = w;
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
    cprintf("%d %d %d %d\n", x, y, cx, cy);
//    argptr(5, (char**)(&content), sizeof(RGB));

    int p;
    argint(5, &p);
    content = (RGB *) p;
    cprintf("Ptr exam %d\n", content);
    /*RGBA color;*/
    /*color.A = 255;*/
    /*color.R = 255;*/
    /*color.G = 0;*/
    /*color.B = 0;*/
    /*drawString(screen, 100, 200, "Hello World!", color);*/
    /*drawMouse(screen, 0, 100, 100);*/
    /*drawMouse(screen, 1, 100, 120);*/

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
       }
    }
    release(&guiKernelLock);
    return 0;
}

int testXXX(RGB * p)
{
    cprintf("%d %d %d", p->R, p->G, p->B);
    cprintf("asdasda");
    return 0;
}

int sys_repaintwindow()
{
    int hwnd;
    argint(0, &hwnd);

    cprintf("%d\n", wndInfoList[hwnd].procPtr);
    cprintf("Draw check %d\n",wndInfoList[hwnd].content);
    RGB* p = wndInfoList[hwnd].content;
    testXXX(p);
    drawRGBContentToContent(screen, p,0, 0, 800, 600);
    //if (proc == 0)
//		switchkvm();
//	else
//		switchuvm(proc);


    // drawString(screen, 100, 200, "Hello World!", color);
    /*drawMouse(screen, 0, 100, 100);*/
    /*drawMouse(screen, 1, 100, 120);*/
   return 0;
}

/*void dispatchMessage(msg *, wndInfo *)*/
/*{*/

/*}*/
