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
    int flag = 1;
    int pid;
    char * argv[] = {"test", "0"};
    switch (msg->msg_type)
    {
    case M_KEY_DOWN:
        if(msg->params[0]==97)
        {
            if (flag == 1)
            {
                printf(1, "init: starting sh\n");
                pid = fork();
                if(pid < 0){
                      exit();
                }
                if(pid == 0){
                  exec("test", argv);
                  printf(1, "init: exec desktop failed\n");
                  exit();
                }
            }
        }
        printf(1, "USER K DOWN%d %d\n", msg->params[0], msg->params[1]);
        break;
    case M_KEY_UP:
        printf(1, "USER K UP%d %d\n", msg->params[0], msg->params[1]);
        break;
    }
}
int
main(void)
{
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

    //api_paint24Bitmap(&wnd, background,(Point){0,0}, (Size){600, 800});
    api_paint24BitmapToContent(&wnd, background, (Point){0,0}, (Point){0,0},
            (Size){600, 800}, (Size){600, 800});
    api_repaint(&wnd);
    printf(1, "safe here\n");
    api_exec(&wnd, &MsgProc);
    return 0;
}
