#include "types.h"
#include "x86.h"
#include "param.h"
#include "defs.h"
#include "msg.h"

//#define MAX_WINDOW_COUNT 50;

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

/*void dispatchMessage(msg *, wndInfo *)*/
/*{*/

/*}*/
