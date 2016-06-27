#include "types.h"
#include "xv6_api.h"
#include "user.h"

int api_createwindow(Window* wnd)
{
    Size size = wnd->size;
    wnd->content = malloc(size.w * size.h * 3);
    // set all content to zero
    memset(wnd->content, 255, size.w * size.h * 3);
    wnd->hWnd = createwindow(wnd->pos.x, wnd->pos.y,
            wnd->size.w, wnd->size.h, wnd->title);
    return wnd->hWnd;
}
