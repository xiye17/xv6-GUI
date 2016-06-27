#include "types.h"
#include "xv6_api.h"
#include "user.h"
#include "gui_base.h"

int api_createwindow(Window* wnd)
{
    Size size = wnd->size;
    wnd->content = malloc(size.w * size.h * 3);
    // set all content to zero
    memset(wnd->content, 255, size.w * size.h * 3);
    wnd->hwnd = createwindow(wnd->pos.x, wnd->pos.y,
            wnd->size.w, wnd->size.h, wnd->title, wnd->content);
    return wnd->hwnd;
}

int api_paint24Bitmap(Window* wnd, struct RGB* img, Point p, Size s)
{
    int x, y;
    x = p.x;
    y = p.y;

    int i;
    struct RGB *t;
    struct RGB *o;
    int max_line = (wnd->size.w - x) < s.w ? (wnd->size.w - x) :s.w;
    for (i = 0; i < s.h; i++) {
        if (y + i > wnd->size.h || y + i < 0) {
            break;
        }
        t = wnd->content + (y + i) * wnd->size.w + x;
        o = img + (s.h - i - 1) * s.w;
        memmove(t, o, max_line * 3);
    }
    return 0;
}

int api_repaint(Window* wnd)
{
    repaintwindow(wnd->hwnd);
    return 0;
}
