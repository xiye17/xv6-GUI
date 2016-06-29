#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "spinlock.h"
#include "gui_base.h"
#include "character.h"
#include "bitmap.h"
#include "mouse_shape.h"

/*ushort SCREEN_WIDTH;*/
/*ushort SCREEN_HEIGHT;*/
/*int screen_size;*/
struct spinlock screen_lock;
struct spinlock buf1_lock;
struct spinlock buf2_lock;

RGB *screen = 0;
RGB *screen_buf1 = 0;
RGB *screen_buf2 = 0;

void initGUI() {
    uint GraphicMem = KERNBASE + 0x1028;
    uint baseAdd = *((uint*)GraphicMem);
    screen = (RGB*)baseAdd;
    SCREEN_WIDTH = *((ushort*)(KERNBASE + 0x1012));
    SCREEN_HEIGHT = *((ushort*)(KERNBASE + 0x1014));
    screen_size = (SCREEN_WIDTH * SCREEN_HEIGHT) * 3;
    screen_buf1 = (RGB*)(baseAdd + screen_size);
    screen_buf2 = (RGB*)(baseAdd + screen_size * 2);
    initlock(&screen_lock, "screen");
    initlock(&buf1_lock, "buffer_1");
    initlock(&buf2_lock, "buffer_2");

    mouse_color[0].G = 0;
    mouse_color[0].B = 0;
    mouse_color[0].R = 0;
    mouse_color[1].G = 200;
    mouse_color[1].B = 200;
    mouse_color[1].R = 200;

    cprintf("@Screen Width:   %d\n", SCREEN_WIDTH);
    cprintf("@Screen Height:  %d\n", SCREEN_HEIGHT);
    cprintf("@Bits per pixel: %d\n",*((uchar*)(KERNBASE+0x1019)));
    cprintf("@Video card drivers initialized successfully.\n");
}

void acquireGUILock(RGB *buf) {
    if (buf == screen) {
        acquire(&screen_lock);
    } else if (buf == screen_buf1) {
        acquire(&buf1_lock);
    } else if (buf == screen_buf2) {
        acquire(&buf2_lock);
    }
}

void releaseGUILock(RGB *buf) {
    if (buf == screen) {
        release(&screen_lock);
    } else if (buf == screen_buf1) {
        release(&buf1_lock);
    } else if (buf == screen_buf2) {
        release(&buf2_lock);
    }
}

void drawPoint(RGB* color, RGB origin) {
    color->R = origin.R;
    color->G = origin.G;
    color->B = origin.B;
}

void drawPointAlpha(RGB* color, RGBA origin) {
    float alpha;
    if (origin.A == 255) {
        color->R = origin.R;
        color->G = origin.G;
        color->B = origin.B;
        return;
    }
    if (origin.A == 0) {
        return;
    }
    alpha = (float)origin.A / 255;
    color->R = color->R * (1 - alpha) + origin.R * alpha;
    color->G = color->G * (1 - alpha) + origin.G * alpha;
    color->B = color->B * (1 - alpha) + origin.B * alpha;
}

int drawCharacter(RGB *buf, int x, int y, char ch, RGBA color) {
    int i, j;
    RGB *t;
    int ord = ch - 0x20;
    if (ord < 0 || ord >= (CHARACTER_NUMBER - 1)) {
        return -1;
    }
    for (i = 0; i < CHARACTER_HEIGHT; i++) {
        if (y + i > SCREEN_HEIGHT || y + i < 0) {
            break;
        }
        for (j = 0; j < CHARACTER_WIDTH; j++) {
            if (character[ord][i][j] == 1) {
                if (x + j > SCREEN_WIDTH || x + j < 0) {
                    break;
                }
                t = buf + (y + i) * SCREEN_WIDTH + x + j;
                drawPointAlpha(t, color);
            }
        }
    }
    return CHARACTER_WIDTH;
}

void drawString(RGB *buf, int x, int y, char *str, RGBA color) {
    int offset_x = 0;

    while (*str != '\0') {
        offset_x += drawCharacter(buf, x + offset_x, y, *str, color);
        str++;
    }
}

int drawCharacterToContent(RGB *buf, int x, int y, int cx, int cy, char ch, RGBA color) {
    int i, j;
    RGB *t;
    int ord = ch - 0x20;
    acquireGUILock(buf);
    if (ord < 0 || ord >= (CHARACTER_NUMBER - 1)) {
        return -1;
    }
    for (i = 0; i < CHARACTER_HEIGHT; i++) {
        if (y + i > cy || y + i < 0) {
            break;
        }
        for (j = 0; j < CHARACTER_WIDTH; j++) {
            if (character[ord][i][j] == 1) {
                if (x + j > cx || x + j < 0) {
                    break;
                }
                t = buf + (y + i) * cx + x + j;
                drawPointAlpha(t, color);
            }
        }
    }
    releaseGUILock(buf);
    return CHARACTER_WIDTH;
}

void drawStringToContent(RGB *buf, int x, int y, int cx, int cy, char *str, RGBA color) {
    int offset_x = 0;
    while (*str != '\0') {
        offset_x += drawCharacterToContent(buf, x + offset_x, y,cx, cy, *str, color);
        str++;
    }
}

void drawImage(RGB *buf, RGBA *img, int x, int y, int width, int height) {
    int i, j;
    RGB *t;
    RGBA *o;
    for (i = 0; i < height; i++) {
        if (y + i > SCREEN_HEIGHT || y + i < 0) {
            break;
        }
        for (j = 0; j < width; j++) {
            if (x + j > SCREEN_WIDTH || x + j < 0) {
                break;
            }
            t = buf + (y + i) * SCREEN_WIDTH + x + j;
            o = img + (height - i) * width + j;
            drawPointAlpha(t, *o);
        }
    }
}

void draw24Image(RGB *buf, RGB *img, int x, int y, int width, int height) {
    /*int i;*/
    /*RGB *t;*/
    /*RGB *o;*/
    /*int max_line = (SCREEN_WIDTH - x) < width ? (SCREEN_WIDTH - x) : width;*/
    /*for (i = 0; i < height; i++) {*/
        /*if (y + i > SCREEN_HEIGHT || y + i < 0) {*/
            /*break;*/
        /*}*/
        /*t = buf + (y + i) * SCREEN_WIDTH + x;*/
        /*o = img + (height - i) * width;*/
        /*cprintf("%d %d\n", t, o);*/
        /*memmove(t, o, max_line * 3);*/
    /*}*/

    int i, j;
    RGB *t , *o;
    acquireGUILock(buf);
    for (i = 0; i < height; ++i)
    {
         for (j = 0; j < width; ++j)
         {
            t = buf + (y + i) * SCREEN_WIDTH + x + j;
            o = img + (height -i - 1) * width +j;
            drawPoint(t, *o);
         }
    }
    releaseGUILock(buf);
}

void drawRGBContentToContent(RGB *buf, RGB *img, int x, int y, int width, int height) {
    int i;
    RGB *t , *o;

    int line = width;
    acquireGUILock(buf);
    if ((SCREEN_WIDTH -x) < line)
        line = SCREEN_WIDTH - x;

    for (i = 0; i < height; ++i)
    {
        t = buf + (y + i) * SCREEN_WIDTH + x;
        o = img + i * width;
        memmove(t, o, line * 3);
    }
    releaseGUILock(buf);
}

void drawRGBContentToContentPart(RGB *buf, RGB *img, int x, int y,
    int bx, int by, int bh, int bw, int h, int w)
{
    int i;
    RGB *t , *o;
    acquireGUILock(buf);
    int line = SCREEN_WIDTH - x;
    if ((bw - bx) < line)
        line = bw -bx;
    if (w < (bw - bx))
        line = w;
    for (i = 0; i < h; ++i)
    {
        t = buf + (y + i) * SCREEN_WIDTH + x;
        o = img + (by + i) * bw + bx;
        memmove(t, o, line * 3);
    }
    releaseGUILock(buf);
}

void drawMouse(RGB *buf, int mode, int x, int y) {
    int i, j;
    RGB *t;
    acquireGUILock(buf);
    for (i = 0; i < MOUSE_HEIGHT; i++) {
        if (y + i > SCREEN_HEIGHT || y + i < 0) {
            break;
        }
        for (j = 0; j < MOUSE_WIDTH; j++) {
            if (x + j > SCREEN_WIDTH || x + j < 0) {
                break;
            }
            uchar temp = mouse_pointer[mode][i][j];
            if (temp) {
                t = buf + (y + i) * SCREEN_WIDTH + x + j;
                drawPoint(t, mouse_color[temp - 1]);
            }
        }
    }
    releaseGUILock(buf);
}

void drawScreenToScreen(RGB* buf, RGB* img) {
    acquireGUILock(buf);
    memmove(buf, img, SCREEN_WIDTH * SCREEN_HEIGHT * 3);
    releaseGUILock(buf);
}

void clearMouse(RGB *buf, RGB *temp_buf, int x, int y) {
    drawRGBContentToContentPart(buf, temp_buf, x, y, x, y, SCREEN_HEIGHT, SCREEN_WIDTH, MOUSE_HEIGHT, MOUSE_WIDTH);
}

void sys_hello() {
    RGB *image;
    int i;
    int h, w;
    argint(0, &i);
    argint(1, &h);
    argint(2, &w);
    cprintf("size: %d * %d", h, w);
    image = (RGB *)i;
    RGBA color;
    color.A = 200;
    color.G = 255;
    draw24Image(screen, image, 0, 0, w, h);
    drawString(screen, 100, 200, "Hello World!", color);
    drawMouse(screen, 0, 100, 100);
    drawMouse(screen, 1, 100, 120);
}
