#include "types.h"
#include "x86.h"
#include "param.h"
#include "defs.h"
#include "msg.h"
#include "xv6_api.h"
#include "gui_base.h"

//struct wndInfo wndInfoList[MAX_WINDOW_COUNT];

/*void*/
/*gui_kernal_init()*/
/*{*/

/*}*/

void
guiKernelHandleMsg(message *msg)
{
    if (msg->msg_type == M_KEY_UP)
    {
        cprintf("KEY UP: %x, %x\n", msg->params[0], msg->params[1]);
    }
    else if (msg->msg_type == M_KEY_DOWN)
    {
        cprintf("KEY DOWN: %x, %x\n", msg->params[0], msg->params[1]);
    }
    else if (msg->msg_type == M_MOUSE_MOVE)
    {
        cprintf("MOUSE MOVE: dx=%d, dy=%d, btn=%x\n", msg->params[0], msg->params[1], msg->params[2]);
    }
    else if (msg->msg_type == M_MOUSE_DOWN)
    {
        cprintf("MOUSE DOWN: btn=%x\n", msg->params[0]);
    }
    else if (msg->msg_type == M_MOUSE_UP)
    {
        cprintf("MOUSE UP: btn=%x\n", msg->params[0]);
    }
    else if (msg->msg_type == M_MOUSE_LEFT_CLICK)
    {
        cprintf("MOUSE LC: btn=%x\n", msg->params[0]);
    }
    else if (msg->msg_type == M_MOUSE_RIGHT_CLICK)
    {
        cprintf("MOUSE RC: btn=%x\n", msg->params[0]);
    }
    else if (msg->msg_type == M_MOUSE_DBCLICK)
    {
        cprintf("MOUSE DC: btn=%x\n", msg->params[0]);
    }
}


int sys_createwindow(void)
{
    int x, y, cx, cy;
    char * title;
    argint(0, &x);
    argint(1, &y);
    argint(2, &cx);
    argint(3, &cy);
    argstr(4, &title);
    cprintf("%s", title);
    /*RGBA color;*/
    /*color.A = 255;*/
    /*color.R = 255;*/
    /*color.G = 0;*/
    /*color.B = 0;*/
    /*drawString(screen, 100, 200, "Hello World!", color);*/
    /*drawMouse(screen, 0, 100, 100);*/
    /*drawMouse(screen, 1, 100, 120);*/
    return 0;
}

/*void dispatchMessage(msg *, wndInfo *)*/
/*{*/

/*}*/
