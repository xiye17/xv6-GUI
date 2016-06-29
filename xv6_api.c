#include "types.h"
#include "xv6_api.h"
#include "user.h"
#include "gui_base.h"
#include "msg.h"
#include "character.h"

static unsigned int g_seed = 0;

int api_fastrand()
{
    printf(1, "G seed %d\n", g_seed);
    ++g_seed;
    g_seed = (214013*g_seed+2531011);
    return (g_seed>>16)&0x7FFF;
}

int api_createwindow(Window* wnd)
{
    Size size = wnd->size;
    wnd->wholeContent = malloc(size.w * (size.h + UTITLE_HEIGHT) * 3);
    wnd->content = wnd->wholeContent + size.w * UTITLE_HEIGHT;
    // set all content to zero
    memset(wnd->content, 255, size.w * size.h * 3);
    wnd->hwnd = createwindow(wnd->pos.x, wnd->pos.y,
            wnd->size.w, wnd->size.h, wnd->title, wnd->wholeContent);
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

int api_paint24BitmapToContent(Window* wnd, struct RGB* img, Point pWnd, Point pBmp, Size sBmp, Size s)
{
    int i;
    struct RGB *t;
    struct RGB *o;

    int max_line = s.w;
    for (i = 0; i < s.h; i++) {
        /*if (y + i > wnd->size.h || y + i < 0) {*/
            /*break;*/
        /*}*/
        t = wnd->content + (pWnd.y + i) * wnd->size.w + pWnd.x;
        // o = img + (s.h - i - 1) * s.w;
        o = img + (sBmp.h - pBmp.y - 1 - i) * sBmp.w + pBmp.x;
        memmove(t, o, max_line * 3);
    }
    return 0;
}

int api_paint24BitmapToContentTransparent(Window* wnd, struct RGB* img,
Point pWnd, Point pBmp, Size sBmp, Size s)
{
    int i, j;
    struct RGB *t;
    struct RGB *o;

    for (i = 0; i < s.h; ++i) {
        for(j = 0; j < s.w; ++j)
        {
            t = wnd->content + (pWnd.y + i) * wnd->size.w + pWnd.x + j;
            o = img + (sBmp.h - pBmp.y - 1 - i) * sBmp.w + pBmp.x + j;

            if(o->R==255 && o->G == 255 && o->B==255)
                continue;
            t->R = o->R;
            t->G = o->G;
            t->B = o->B;
        }
    }
    return 0;
}

int api_paintContentToContent(Window* wnd, struct RGB* cot, Point pWnd, Point pCot, Size sCot, Size s)
{
    int i;
    struct RGB *t;
    struct RGB *o;
    int max_line = s.w;
    for (i = 0; i < s.h; i++) {
        /*if (y + i > wnd->size.h || y + i < 0) {*/
            /*break;*/
        /*}*/
        t = wnd->content + (pWnd.y + i) * wnd->size.w + pWnd.x;
        // o = img + (s.h - i - 1) * s.w;
        o = cot + (pCot.y +  i) * sCot.w + pCot.x;
        memmove(t, o, max_line * 3);
    }
    return 0;
}

int api_repaint(Window* wnd)
{
    repaintwindow(wnd->hwnd);
    return 0;
}

int api_update(Window* wnd, Rect rect)
{
    updatewindow(wnd->hwnd, rect.x, rect.y, rect.h, rect.w);
    return 0;
}

int api_exec(Window* wnd, ProcFun pf)
{
    message msg;
    int r = 0;
    while(1)
    {
        r = getmessage(wnd->hwnd, &msg);
        if(r)
        {
            pf(&msg);
        }
    }
    return -1;
}

int api_settimer(Window* wnd, int interval)
{
    settimer(wnd->hwnd, interval);
    return 0;
}

int api_drawPointAlpha(RGB* color, RGBA origin) {
    float alpha;
    if (origin.A == 255) {
        color->R = origin.R;
        color->G = origin.G;
        color->B = origin.B;
        return 0;
    }
    if (origin.A == 0) {
        return 0;
    }
    alpha = (float)origin.A / 255;
    color->R = color->R * (1 - alpha) + origin.R * alpha;
    color->G = color->G * (1 - alpha) + origin.G * alpha;
    color->B = color->B * (1 - alpha) + origin.B * alpha;
    return 0;
}

int api_drawCharacter(Window *wnd, int x, int y, char ch, RGBA color) {
    int i, j;
    RGB *t;
    RGB * buf =wnd->content;
    int ord = ch - 0x20;
    if (ord < 0 || ord >= (CHARACTER_NUMBER - 1)) {
        return -1;
    }
    for (i = 0; i < CHARACTER_HEIGHT; i++) {
        if (y + i > wnd->size.h || y + i < 0) {
            break;
        }
        for (j = 0; j < CHARACTER_WIDTH; j++) {
            if (character[ord][i][j] == 1) {
                if (x + j > wnd->size.w || x + j < 0) {
                    break;
                }
                t = buf + (y + i) * wnd->size.w + x + j;
                api_drawPointAlpha(t, color);
            }
        }
    }
    return CHARACTER_WIDTH;

}

int api_drawRect(Window *wnd, Point p, Size s, RGB color)
{
    int i, j;
    RGB * b = wnd->content;
    RGB * o;
    for(i = 0; i < s.h; ++i)
    {
        for (j = 0; j < s.w; ++j)
        {
            o = b + (p.y + i) * wnd->size.w + p.x + j;
            *o = color;
        }
    }
    return 0;
}

int api_drawString(Window *wnd, int x, int y, char *str, RGBA color) {
    int offset_x = 0;

    while (*str != '\0') {
        offset_x += api_drawCharacter(wnd, x + offset_x, y, *str, color);
        str++;
   }

   return 0;
}

int api_drawButton(Window *wnd, Point p, Size s, char * str)
{
    RGB bColor;
    bColor.R = 9;
    bColor.G = 163;
    bColor.B = 220;
    api_drawRect(wnd, p, s, bColor);
    api_drawString(wnd, p.x + 10, p.y + 10, str, (RGBA){255,255,255,255});
    return 0;
}

int api_destroywindow(Window *wnd) {
    destroywindow(wnd->hwnd);

    return 0;
}
