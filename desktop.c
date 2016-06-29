//Desktop

#include "types.h"
#include "color.h"
#include "user.h"
#include "fcntl.h"
#include "msg.h"

//define the xv6 param
#include "xv6_api.h"


Window wnd;
#define  AppCount 5
#define  IconSize 80
char * appName[] = {"test.bmp", "plane.bmp",
    "clock.bmp", "editor.bmp", "painter.bmp"};
Point appPos[AppCount];
RGB IconBuffer[IconSize * IconSize];

int flag = 0;
void MsgProc(struct message * msg)
{
    int pid;
    char * argv[] = {"xxxxxxxxxxxxxxxxxxxxxxx", "0"};
    int i;

    int x;
    int y;
    int clicked;
    switch (msg->msg_type)
    {
    case M_KEY_DOWN:
       printf(1, "USER K DOWN%d %d\n", msg->params[0], msg->params[1]);
        break;
    case M_KEY_UP:
        printf(1, "USER K UP%d %d\n", msg->params[0], msg->params[1]);
        break;
    case M_TIMER:
        break;
    case M_MOUSE_DBCLICK:
        x = msg->params[0];
        y = msg->params[1];
        printf(1, "DES DBC %d %d\n", x, y);
        for(i = 0; i < AppCount; ++i)
        {
            int minX, minY, maxX, maxY;
            minX = appPos[i].x;
            minY = appPos[i].y;
            maxX = appPos[i].x + IconSize;
            maxY = appPos[i].y + IconSize;

            if(x > minX && x < maxX && y > minY && y <maxY)
            {
                clicked = i;
                break;
            }
        }

        if (clicked < AppCount)
        {
            if (clicked == 0)
                flag = flag + 1;
            pid = fork();
            if(pid == 0){
                argv[1][0] = flag + '0';
                strcpy(argv[0],appName[i]);
                int len = strlen(appName[i]);
                len = len - 4;
                char temp[len + 1];
                int j;
                for (j = 0; j < len; ++j)
                    temp[j] = appName[i][j];
                temp[len] = '\0';

                printf(1, "Test %s\n", temp);
                exec(temp, argv);
                exit();
             }
        }
        break;
    }
}

void desktopInit()
{
    int i;

    for (i = 0; i < AppCount; ++i)
    {
        int h, w;
        read24BitmapFile(appName[i], IconBuffer, &h, &w);
        printf(1, "Icon %d, %d", h, w);
        appPos[i].x = 50;
        appPos[i].y = 20 + 120 * i;
        api_paint24BitmapToContent(&wnd, IconBuffer, appPos[i], (Point){0, 0},(Size){80,80}, (Size){80,80});
       // api_paint24Bitmap(&wnd, IconBuffer, appPos[i],(Size){55,40});
    }
}

int
main(void)
{
    wnd.pos.x = 0;
    wnd.pos.y = 0;
    wnd.size.w = USCREEN_WIDTH;
    wnd.size.h = USCREEN_HEIGHT;
    wnd.title = "desktop";

    RGB * background = malloc(800 * 600 * 3);
    api_createwindow(&wnd);

    char filename[] = "desktop.bmp";
    int h, w;
    read24BitmapFile(filename, background, &h, &w);

    api_paint24Bitmap(&wnd, background,(Point){0,0}, (Size){600, 800});
    //api_paint24BitmapToContent(&wnd, background, (Point){0,0}, (Point){0,0},
    //        (Size){600, 800}, (Size){600, 800});
    desktopInit();
    api_repaint(&wnd);
    printf(1, "safe here\n");
    api_exec(&wnd, &MsgProc);
    return 0;
}
