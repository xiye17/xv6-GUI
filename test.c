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
main(void)
{
    wnd.pos.x = 0;
    wnd.pos.y = 0;
    wnd.size.w = 300;
    wnd.size.h = 300;
    wnd.title = "windowtest";

    api_createwindow(&wnd);
    api_repaint(&wnd);
    printf(1, "Wana Die");
    printf(1, "safe here\n");
    api_exec(&wnd, &MsgProc);
    return 0;
}
