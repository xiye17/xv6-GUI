//Desktop

#include "types.h"
#include "color.h"
#include "user.h"
#include "fcntl.h"
#include "msg.h"

//define the xv6 param
#include "xv6_api.h"
Window wnd;

void MsgProc(struct message * msg)
{
    switch (msg->msg_type)
    {
    case M_KEY_DOWN:
        break;
    case M_KEY_UP:
        break;
    }
}
int
main(int argc, char *argv[])
{
    int pra = argv[1][0] - '0';
    wnd.pos.x = pra * 50;
    wnd.pos.y = pra * 50;
    wnd.size.w = 300;
    wnd.size.h = 300;
    wnd.title = "windowtest";
    api_createwindow(&wnd);
    memset(wnd.content, pra * 50, wnd.size.w * wnd.size.h * 3);
    /*RGBA color;*/
    /*color.R =255;*/
    /*color.G =0;*/
    /*color.B =0;*/
    /*color.A =255;*/
    /*api_drawString(&wnd, 50, 50, "2333", color);*/
    api_repaint(&wnd);
    api_exec(&wnd, &MsgProc);
    return 0;
}
