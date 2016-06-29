#include "types.h"
#include "color.h"
#include "user.h"
#include "fcntl.h"
#include "msg.h"
#include "xv6_api.h"
#include "kbd.h"
Window wnd;
int pos_x = 0;
int pos_y = 40;
RGBA black={255,0,0,0};
char firstPage[10*40]={'\0'};
int first = 0;
int second = 0;
int page=0;
char secondPage[10*40]={'\0'};
RGB white={255,255,255};
RGB *background;
char null[2]="  ";
char pgdn[5]="PgDn";
char save[5]="Save";
#define CHARWIDTH 10
#define CHARHEIGHT 20
#define SCREENWIDTH 400
#define SCREENHEIGHT 300 
void MsgProc(struct message * msg)
{
    switch(msg->msg_type)
    {
        case M_KEY_DOWN:
            if (msg->params[0]==226)
            {
                if (pos_y!=40)
                {
                    pos_y-=CHARHEIGHT;
                    if (page==0)
                    {
                        first-=39;
                    }
                    else
                    {
                        second-=39;    
                    }
                }
            }
            else if (msg->params[0]==227)
            {
                if(pos_y!=SCREENHEIGHT-CHARHEIGHT)
                {
                    pos_y+=CHARHEIGHT;
                    if (page==0)
                    {
                        first+=39;
                    }
                    else
                    {
                        second+=39;    
                    }
                }
            }
            else if (msg->params[0]==228)
            {
                if (msg->params[1]==1)
                {
                    if (page==0)
                    {
                        page = 1;
                        pos_x=0;
                        pos_y=40;
                        api_paint24BitmapToContent(&wnd, background, (Point){0,0}, (Point){0,0},(Size){300,400},(Size){300,400});
                        api_drawButton(&wnd,(Point){0,0},(Size){30,5*CHARWIDTH},pgdn);
                        api_drawButton(&wnd,(Point){6*CHARWIDTH,0},(Size){30,5*CHARWIDTH},save);
                        api_repaint(&wnd);
                        int i = 0;
                        for (i=0;i<=second;i++)
                        {
                             api_drawCharacter(&wnd,pos_x,pos_y,secondPage[i],black);
                             pos_x+=CHARWIDTH;
                             api_update(&wnd,(Rect){pos_x-CHARWIDTH,pos_y,CHARWIDTH,CHARHEIGHT});
                             if (pos_x==39*CHARWIDTH)
                             {
                                 pos_x=0;
                                 pos_y+=CHARHEIGHT;
                             }
                        }
                        pos_x-=CHARWIDTH;
                    }
                    else if (page==1)
                    {
                        page = 0;
                        pos_x=0;
                        pos_y=40;
                        api_paint24BitmapToContent(&wnd, background, (Point){0,0}, (Point){0,0},(Size){300,400},(Size){300,400});
                        api_drawButton(&wnd,(Point){0,0},(Size){30,5*CHARWIDTH},pgdn);
                        api_drawButton(&wnd,(Point){6*CHARWIDTH,0},(Size){30,5*CHARWIDTH},save);
                        api_repaint(&wnd);
                        int i = 0;
                        for (i=0;i<=first;i++)
                        {
                             api_drawCharacter(&wnd,pos_x,pos_y,firstPage[i],black);
                             pos_x+=CHARWIDTH;
                             api_update(&wnd,(Rect){pos_x-CHARWIDTH,pos_y,CHARWIDTH,CHARHEIGHT});
                             if (pos_x==39*CHARWIDTH)
                             {
                                 pos_x=0;
                                 pos_y+=CHARHEIGHT;
                             }
                        }
                        pos_x-=CHARWIDTH;
                    }
                }
                else if (pos_x!=0)
                {
                    pos_x-=CHARWIDTH;
                    if (page==0)
                    {
                        first-=1;
                    }
                    else
                    {
                        second-=1;    
                    }
                }
            }
            else if (msg->params[0]==229)
            {
                if (pos_x!=SCREENWIDTH-CHARWIDTH)
                {
                    pos_x+=CHARWIDTH;
                    if (page==0)
                    {
                        first+=1;
                    }
                    else
                    {
                        second+=1;    
                    }
                }
            }
            else if(msg->params[1]==1)
            {
                if (msg->params[0]>31)
                {
                    api_drawRect(&wnd,(Point){pos_x,pos_y},(Size){SCREENHEIGHT,SCREENWIDTH},white);
                    api_drawCharacter(&wnd,pos_x,pos_y,(char)(msg->params[0]-32),black);
                    pos_x+=CHARWIDTH;
                    if (page==0)
                    {
                        firstPage[first]=(char)(msg->params[0]-32);
                        first++;
                    }
                    else if (page == 1)
                    {
                        secondPage[second]=(char)(msg->params[0]-32);
                        second++;
                    }
                    api_update(&wnd,(Rect){pos_x-CHARWIDTH,pos_y,CHARWIDTH,CHARHEIGHT});
                }
            }
            else 
            {
                if (msg->params[0]>31)
                {
                    api_drawRect(&wnd,(Point){pos_x,pos_y},(Size){SCREENHEIGHT,SCREENWIDTH},white);
                    api_drawCharacter(&wnd,pos_x,pos_y,(char)msg->params[0],black);
                    pos_x+=CHARWIDTH;
                    if (page==0)
                    {
                        firstPage[first]=(char)(msg->params[0]);
                        first++;
                    }
                    else if (page == 1)
                    {
                        secondPage[second]=(char)(msg->params[0]);
                        second++;
                    }
                    api_update(&wnd,(Rect){pos_x-CHARWIDTH,pos_y,CHARWIDTH,CHARHEIGHT});
                }
            }
            if (pos_x>SCREENWIDTH-2*CHARWIDTH)
            {
                pos_x=0;
                pos_y+=CHARHEIGHT;
                if(pos_y == SCREENHEIGHT-CHARHEIGHT)
                {
                    page+=1;
                    pos_x=0;
                    pos_y=40;
                    api_paint24BitmapToContent(&wnd, background, (Point){0,0}, (Point){0,0},(Size){300,400},(Size){300,400});
                    api_drawButton(&wnd,(Point){0,0},(Size){30,5*CHARWIDTH},pgdn);
                    api_drawButton(&wnd,(Point){6*CHARWIDTH,0},(Size){30,5*CHARWIDTH},save);
                    api_repaint(&wnd);
                }
            }
            break;
        case M_KEY_UP:
            break;
        case M_MOUSE_LEFT_CLICK:
            if ((msg->params[0]<5*CHARWIDTH)&&(msg->params[0]>0)&&(msg->params[1]>0)&&(msg->params[1]<20))
            {
                if (page==0)
                    {
                        page = 1;
                        pos_x=0;
                        pos_y=40;
                        api_paint24BitmapToContent(&wnd, background, (Point){0,0}, (Point){0,0},(Size){300,400},(Size){300,400});
                        api_drawButton(&wnd,(Point){0,0},(Size){30,5*CHARWIDTH},pgdn);
                        api_drawButton(&wnd,(Point){6*CHARWIDTH,0},(Size){30,5*CHARWIDTH},save);
                        api_repaint(&wnd);
                        int i = 0;
                        for (i=0;i<=second;i++)
                        {
                             api_drawCharacter(&wnd,pos_x,pos_y,secondPage[i],black);
                             pos_x+=CHARWIDTH;
                             api_update(&wnd,(Rect){pos_x-CHARWIDTH,pos_y,CHARWIDTH,CHARHEIGHT});
                             if (pos_x==39*CHARWIDTH)
                             {
                                 pos_x=0;
                                 pos_y+=CHARHEIGHT;
                             }
                        }
                        pos_x-=CHARWIDTH;
                    }
                    else if (page==1)
                    {
                        page = 0;
                        pos_x=0;
                        pos_y=40;
                        api_paint24BitmapToContent(&wnd, background, (Point){0,0}, (Point){0,0},(Size){300,400},(Size){300,400});
                        api_drawButton(&wnd,(Point){0,0},(Size){30,5*CHARWIDTH},pgdn);
                        api_drawButton(&wnd,(Point){6*CHARWIDTH,0},(Size){30,5*CHARWIDTH},save);
                        api_repaint(&wnd);
                        int i = 0;
                        for (i=0;i<=first;i++)
                        {
                             api_drawCharacter(&wnd,pos_x,pos_y,firstPage[i],black);
                             pos_x+=CHARWIDTH;
                             api_update(&wnd,(Rect){pos_x-CHARWIDTH,pos_y,CHARWIDTH,CHARHEIGHT});
                             if (pos_x==39*CHARWIDTH)
                             {
                                 pos_x=0;
                                 pos_y+=CHARHEIGHT;
                             }
                        }
                        pos_x-=CHARWIDTH;
                    }
            }
            break;
        case M_CLOSE_WINDOW:
            free(background);
            api_destroywindow(&wnd);
            break;
    }
}
int 
main(void)
{
    wnd.pos.x = 200;
    wnd.pos.y = 150;
    wnd.size.w = 400;
    wnd.size.h = 300;
    wnd.title = "Editor";
    api_createwindow(&wnd);
    background = malloc(400*300*3);
    int i = 0;
    for (i = 0; i < 400*300;i++)
    {
        background[i].R = 255;
        background[i].G = 255;
        background[i].B = 255;
    }
    api_paint24BitmapToContent(&wnd, background, (Point){0,0}, (Point){0,0},(Size){300,400},(Size){300,400});
    //api_drawButton(&wnd,(Point){0,0},(Size){20,SCREENWIDTH},null);
    api_drawButton(&wnd,(Point){0,0},(Size){30,5*CHARWIDTH},pgdn);
    api_drawButton(&wnd,(Point){6*CHARWIDTH,0},(Size){30,5*CHARWIDTH},save);
    api_repaint(&wnd);
    api_exec(&wnd,&MsgProc);
    return 0;
}
