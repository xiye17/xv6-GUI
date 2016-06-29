//Desktop

#include "types.h"
#include "color.h"
#include "user.h"
#include "fcntl.h"
#include "msg.h"

//define the xv6 param
#include "xv6_api.h"

const int pointSize = 15;

Window wnd;

RGB *background;

int flag = 0;

int isMouseInButton(int x, int y) {
    if (420 < x && x < 490 && 260 < y && y < 290) {
        return 1;
    }
    else {
        return 0;
    }
}

void MsgProc(struct message *msg) {

    //int pid;
    //char * argv[] = {"test", "0"};

    static int mouseDown = 0;
    static int isPencil = 1;
    switch (msg->msg_type) {
        case M_MOUSE_DOWN:
            if (!isMouseInButton(msg->params[0], msg->params[1])) {
                mouseDown = 1;
                if (isPencil) {
                    api_drawRect(&wnd, (Point) {msg->params[0] - pointSize / 2, msg->params[1] - pointSize / 2},
                                 (Size) {pointSize, pointSize}, (RGB) {0, 0, 0});
                }
                else {
                    api_drawRect(&wnd, (Point) {msg->params[0] - pointSize / 2, msg->params[1] - pointSize / 2},
                                 (Size) {pointSize, pointSize}, (RGB) {255, 255, 255});
                }
                api_repaint(&wnd);
            }
            break;
        case M_MOUSE_MOVE:
            if (!isMouseInButton(msg->params[0], msg->params[1]) && mouseDown) {
                if (isPencil) {
                    api_drawRect(&wnd, (Point) {msg->params[0] - pointSize / 2, msg->params[1] - pointSize / 2},
                                 (Size) {pointSize, pointSize}, (RGB) {0, 0, 0});
                }
                else {
                    api_drawRect(&wnd, (Point) {msg->params[0] - pointSize / 2, msg->params[1] - pointSize / 2},
                                 (Size) {pointSize, pointSize}, (RGB) {255, 255, 255});
                }
                api_repaint(&wnd);
            }
            break;
        case M_MOUSE_UP:
            mouseDown = 0;
            break;
        case M_MOUSE_LEFT_CLICK:
            if (isMouseInButton(msg->params[0], msg->params[1])) {
                if (isPencil) {
                    api_drawButton(&wnd, (Point) {420, 260}, (Size) {30, 70}, "eraser");
                }
                else {
                    api_drawButton(&wnd, (Point) {420, 260}, (Size) {30, 70}, "pencil");
                }
                isPencil = 1 - isPencil;
                api_repaint(&wnd);
            }
            break;
    }
}

int
main(void) {
    wnd.pos.x = 100;
    wnd.pos.y = 100;
    wnd.size.w = 500;
    wnd.size.h = 300;
    wnd.title = "DrawerApp";

    char backgroundFilename[] = "DrawerAppBackground.bmp";

    background = malloc(270 * 500 * 3);

    int h, w;
    read24BitmapFile(backgroundFilename, background, &h, &w);

    api_createwindow(&wnd);

    api_paint24BitmapToContent(&wnd, background, (Point) {0, 30}, (Point) {0, 0},
                               (Size) {270, 500}, (Size) {270, 500});


    api_drawButton(&wnd, (Point) {420, 260}, (Size) {30, 70}, "pencil");
    api_repaint(&wnd);
    printf(1, "safe here\n");
    api_exec(&wnd, &MsgProc);
    return 0;
}
