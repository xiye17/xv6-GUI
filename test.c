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
    printf(1, "X Dog %d %d", wnd.pos.x, wnd.pos.y);
    api_createwindow(&wnd);
    memset(wnd.content, pra * 50, wnd.size.w * wnd.size.h * 3);
    api_repaint(&wnd);
    printf(1, "Wana Die");
    printf(1, "safe here\n");
    api_exec(&wnd, &MsgProc);
    return 0;
}