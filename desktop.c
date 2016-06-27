//Desktop

#include "types.h"
#include "color.h"
#include "user.h"
#include "fcntl.h"

//define the xv6 param
#include "xv6_api.h"

int
main(void)
{
    Window wnd;
    wnd.pos.x = 0;
    wnd.pos.y = 0;
    wnd.size.w = USCREEN_WIDTH;
    wnd.size.h = USCREEN_HEIGHT;
    wnd.title = "window1";

    int hwnd = api_createwindow(&wnd);
    /*char filename[] = "desktop.bmp";*/
    /*RGB * background;*/
    /*int h, w;*/
    /*read24BitmapFile(filename, background, &h, &w);*/
    hwnd = 1;
    while(hwnd);
    return 0;
}
