//Desktop
#include "types.h"
#include "color.h"
#include "user.h"
#include "fcntl.h"
#include "msg.h"

//define the xv6 param
#include "xv6_api.h"


Window wnd;

RGB * numbers[10];
RGB * separator;

void drawNumberToContent(int minute, int second)
{
    static int beforefirst = 0;
    static int beforesecond = 0;
    static int beforethird = 0;
    static int beforefourth = 0;

    int nowfirst = second % 10;
    int nowsecond = second / 10;
    int nowthird = minute % 10;
    int nowfourth = minute / 10;

    if (beforefirst != nowfirst)
    {
        api_paint24BitmapToContent(&wnd, numbers[nowfirst], (Point){185,35}, (Point){0,0},
                                   (Size){55, 40}, (Size){55, 40});
        beforefirst = nowfirst;
    }
    if (beforesecond != nowsecond)
    {
        api_paint24BitmapToContent(&wnd, numbers[nowsecond], (Point){145,35}, (Point){0,0},
                                   (Size){55, 40}, (Size){55, 40});
        beforesecond = nowsecond;
    }
    if (beforethird != nowthird)
    {
        api_paint24BitmapToContent(&wnd, numbers[nowthird], (Point){65,35}, (Point){0,0},
                                   (Size){55, 40}, (Size){55, 40});
        beforethird = nowthird;
    }
    if (beforefourth != nowfourth)
    {
        api_paint24BitmapToContent(&wnd, numbers[nowfourth], (Point){25,35}, (Point){0,0},
                                   (Size){55, 40}, (Size){55, 40});
        beforefourth = nowfourth;
    }
}

int flag = 0;
void MsgProc(struct message * msg)
{
    static int second = 0;
    static int minute = 0;
    int i;
    switch (msg->msg_type)
    {
        case M_TIMER:
            if (second < 59)
            {
                second++;
            }
            else
            {
                second = 0;
                minute++;
            }
            drawNumberToContent(minute, second);
            api_repaint(&wnd);
            break;
        case M_CLOSE_WINDOW:
            free(separator);
            for (i = 0; i < 10; i++)
            {
               free(numbers[i]);
            }
            api_destroywindow(&wnd);
            break;
    }
}

int
main(void)
{
    wnd.pos.x = 100;
    wnd.pos.y = 100;
    wnd.size.w = 250;
    wnd.size.h = 100;
    wnd.title = "TimerApp";

    char* numberFilename[10] = {"0.bmp", "1.bmp", "2.bmp", "3.bmp", "4.bmp", "5.bmp", "6.bmp", "7.bmp", "8.bmp", "9.bmp"};
    char  separatorFilename[] = "separator.bmp";

    int i;

    for (i = 0; i < 10; i++)
    {
        numbers[i] = malloc(55 * 40 * 3);
    }
    separator = malloc(55 * 40 * 3);

    api_createwindow(&wnd);

    int h, w;
    for (i = 0; i < 10; i++)
    {
        read24BitmapFile(numberFilename[i], numbers[i], &h, &w);
    }

    read24BitmapFile(separatorFilename, separator, &h, &w);

    

    api_paint24BitmapToContent(&wnd, numbers[0], (Point){185,35}, (Point){0,0},
                               (Size){55, 40}, (Size){55, 40});
    api_paint24BitmapToContent(&wnd, numbers[0], (Point){145,35}, (Point){0,0},
                               (Size){55, 40}, (Size){55, 40});
    api_paint24BitmapToContent(&wnd, separator, (Point){105,35}, (Point){0,0},
                               (Size){55, 40}, (Size){55, 40});
    api_paint24BitmapToContent(&wnd, numbers[0], (Point){65,35}, (Point){0,0},
                               (Size){55, 40}, (Size){55, 40});
    api_paint24BitmapToContent(&wnd, numbers[0], (Point){25,35}, (Point){0,0},
                               (Size){55, 40}, (Size){55, 40});

    api_settimer(&wnd, 1000);
    api_repaint(&wnd);
    printf(1, "safe here\n");
    api_exec(&wnd, &MsgProc);
    return 0;
}
