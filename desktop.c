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

    RGB * background = malloc(800 * 600 * 3);
    api_createwindow(&wnd);
    char filename[] = "desktop.bmp";
    int h, w;
    read24BitmapFile(filename, background, &h, &w);


    Point pDraw = {0, 0};
    Size sDraw = {600, 800};
    api_paint24Bitmap( &wnd, background, pDraw, sDraw);

    api_repaint(&wnd);
    printf(1, "safe here\n");
    while(1 >= 0);
    return 0;
}
