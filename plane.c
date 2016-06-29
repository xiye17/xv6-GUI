#include "types.h"
#include "color.h"
#include "user.h"
#include "fcntl.h"
#include "msg.h"
#include "xv6_api.h"
Window wnd;
#define PLANE_HEIGHT 47
#define PLANE_WIDTH 45
#define BACKGROUNDWIDTH 480
#define BACKGROUNDHEIGHT 500
int plane_x;
int plane_y;
Point enemy[6]={{0,0},{50,0},{100,20},{150,40},{200,20},{250,40}};
Point bullets[30];
RGB * background ;
RGB * plane ;
RGB * bigEnemy ;
RGB * middleEnemy;
RGB * smallEnemy;
RGB * bullet1;
int backgroundh = 0;
char filename[] = "background.bmp";
char heroplane[] = "hero.bmp";
char bigplane[] = "enemy_big.bmp";
char middleplane[] = "enemy_middle.bmp";
char smallplane[] = "enemy_small.bmp";
char bullet[] = "bullet1.bmp";
int bulletNum = 0;
int location = 0;
int tmp = 0;
int enemy_status[6]={0,0,0,0,0,0};
int enemyWidth[6] = {64,46,46,45,45,45};
int enemyHeight[6] = {66,50,50,59,59,59};
void MsgProc(struct message *msg)
{
    switch(msg->msg_type)
    {
        case M_KEY_DOWN:
            if (msg->params[0]==226)
            {
                plane_y-=5;
                if (plane_y < 0)
                {
                    plane_y = 0;
                }
            }
            else if (msg->params[0]==227)
            {
                plane_y+=5;
                if (plane_y > 500-PLANE_HEIGHT)
                {
                    plane_y=500 - PLANE_HEIGHT;
                }
            }
            else if (msg->params[0]==228)
            {
                plane_x-=5;
                if (plane_x <0)
                {
                    plane_x=0;
                }
            }
            else if (msg->params[0]==229)
            {
                plane_x+=5;
                if (plane_x > 300-PLANE_WIDTH)
                {
                    plane_x = 300-PLANE_WIDTH;
                }
            }
            backgroundh+=10;
            backgroundh%=800;
            api_paint24BitmapToContentTransparent(&wnd, background, (Point){0,0},(Point){0,backgroundh},(Size){1600,BACKGROUNDWIDTH},(Size){BACKGROUNDHEIGHT,BACKGROUNDWIDTH});
            api_paint24BitmapToContentTransparent(&wnd, plane, (Point){plane_x,plane_y}, (Point){0,0}, (Size){273,PLANE_WIDTH},(Size){PLANE_HEIGHT,PLANE_WIDTH});
            api_paint24BitmapToContentTransparent(&wnd, bigEnemy, (Point){enemy[0].x,enemy[0].y}, (Point){0,enemy_status[0]*enemyHeight[0]},(Size){401,64},(Size){enemyHeight[0],enemyWidth[0]});
            api_paint24BitmapToContentTransparent(&wnd, middleEnemy, (Point){enemy[1].x,enemy[1].y}, (Point){0,enemy_status[1]*enemyHeight[1]},(Size){311,46},(Size){enemyHeight[1],enemyWidth[1]});
            api_paint24BitmapToContentTransparent(&wnd, middleEnemy, (Point){enemy[2].x,enemy[2].y}, (Point){0,enemy_status[2]*enemyHeight[2]},(Size){311,46},(Size){enemyHeight[2],enemyWidth[2]});
            api_paint24BitmapToContentTransparent(&wnd, smallEnemy, (Point){enemy[3].x,enemy[3].y}, (Point){0,enemy_status[3]*enemyHeight[3]},(Size){340,45},(Size){enemyHeight[3],enemyWidth[3]});
            api_paint24BitmapToContentTransparent(&wnd, smallEnemy, (Point){enemy[4].x,enemy[4].y}, (Point){0,enemy_status[4]*enemyHeight[4]},(Size){340,45},(Size){enemyHeight[4],enemyWidth[4]});
            api_paint24BitmapToContentTransparent(&wnd, smallEnemy, (Point){enemy[5].x,enemy[5].y}, (Point){0,enemy_status[5]*enemyHeight[5]},(Size){340,45},(Size){enemyHeight[5],enemyWidth[5]});
            int i = 0;
            for (i = 0; i < bulletNum;i++)
            {
                api_paint24BitmapToContentTransparent(&wnd,bullet1,(Point){bullets[i].x,bullets[i].y},(Point){0,0},(Size){22,15},(Size){22,15});
            }
            api_repaint(&wnd);
            break;
        case M_TIMER:
            //refresh the bullets
            for (i = 0; i < bulletNum; i++)
            {
                bullets[i].y -= 10;
            }
            for (i =0;i<bulletNum;i++)
            {
                if (bullets[i].y <0)
                {
                    int j = i;
                    for (j = i; j < bulletNum;j++)
                    {
                        bullets[j].x = bullets[j+1].x;
                        bullets[j].y = bullets[j+1].y;
                    }
                    bulletNum-=1;
                    i--;
                }
            }
            tmp+=1;
            tmp%=3;
            if (tmp == 0)
            {
                bullets[bulletNum].x = plane_x+PLANE_WIDTH/2-7;
                bullets[bulletNum].y = plane_y-7;
                bulletNum+=1;
            }
            int j;
            location+=30;
            location%=250;
            //refresh the enemy
            for (i=0;i<6;i++)
            {
                if (enemy_status[i]!=0)
                {
                    enemy_status[i]+=1;
                }
                if (enemy_status[0]>5)
                {
                    enemy_status[0]=0;
                    enemy[0].x = location;
                    enemy[0].y = 0;
                    location+=30;
                    location%=250;
                }
                else if ((i!=0)&&(enemy_status[i]>4))
                {
                    enemy_status[i]=0;
                    enemy[i].x = location;
                    enemy[i].y = 0;
                    location+=30;
                    location%=250;
                }
                if (enemy_status[i]==0)
                {
                    enemy[i].y+=10;
                    if (enemy[i].y > (500-enemyHeight[i]))
                    {
                        enemy[i].y = 0;
                        enemy[i].x = location;
                        location+=30;
                        location%=250;
                    }
                }
            }
            //crash
            for (i=0; i < 6; i++)
            {
                if (enemy_status[i]!=0) continue;
                for (j = 0; j<bulletNum;j++)
                {
                    if (((bullets[j].x<enemy[i].x+enemyWidth[i])&&(bullets[j].x>(enemy[i].x-9))&&(bullets[j].y>enemy[i].y-21)&&(bullets[j].y)<enemy[i].y+enemyHeight[i]))
                    {
                        enemy_status[i]=1;
                        for (; j < bulletNum-1;j++)
                        {
                            bullets[j].x = bullets[j+1].x;
                            bullets[j].y = bullets[j+1].y;
                        }
                        bulletNum-=1;
                    }
                }
            }
            backgroundh+=10;
            backgroundh%=800;
            api_paint24BitmapToContentTransparent(&wnd, background, (Point){0,0},(Point){0,backgroundh},(Size){1600,480},(Size){500,480});
            api_paint24BitmapToContentTransparent(&wnd, plane, (Point){plane_x,plane_y}, (Point){0,0}, (Size){273,PLANE_WIDTH},(Size){PLANE_HEIGHT,PLANE_WIDTH});
            api_paint24BitmapToContentTransparent(&wnd, bigEnemy, (Point){enemy[0].x,enemy[0].y}, (Point){0,enemy_status[0]*enemyHeight[0]},(Size){401,64},(Size){enemyHeight[0],enemyWidth[0]});
            api_paint24BitmapToContentTransparent(&wnd, middleEnemy, (Point){enemy[1].x,enemy[1].y}, (Point){0,enemy_status[1]*enemyHeight[1]},(Size){311,46},(Size){enemyHeight[1],enemyWidth[1]});
            api_paint24BitmapToContentTransparent(&wnd, middleEnemy, (Point){enemy[2].x,enemy[2].y}, (Point){0,enemy_status[2]*enemyHeight[2]},(Size){311,46},(Size){enemyHeight[2],enemyWidth[2]});
            api_paint24BitmapToContentTransparent(&wnd, smallEnemy, (Point){enemy[3].x,enemy[3].y}, (Point){0,enemy_status[3]*enemyHeight[3]},(Size){340,45},(Size){enemyHeight[3],enemyWidth[3]});
            api_paint24BitmapToContentTransparent(&wnd, smallEnemy, (Point){enemy[4].x,enemy[4].y}, (Point){0,enemy_status[4]*enemyHeight[4]},(Size){340,45},(Size){enemyHeight[4],enemyWidth[4]});
            api_paint24BitmapToContentTransparent(&wnd, smallEnemy, (Point){enemy[5].x,enemy[5].y}, (Point){0,enemy_status[5]*enemyHeight[5]},(Size){340,45},(Size){enemyHeight[5],enemyWidth[5]});
            for (i = 0; i < bulletNum;i++)
            {
                api_paint24BitmapToContentTransparent(&wnd,bullet1,(Point){bullets[i].x,bullets[i].y},(Point){0,0},(Size){22,15},(Size){22,15});
            }
            api_repaint(&wnd);
            break;
        case M_CLOSE_WINDOW:
            free(background);
            free(plane);
            free(bigEnemy);
            free(middleEnemy);
            free(smallEnemy);
            free(bullet1);
            api_destroywindow(&wnd);
            break;
    }
    
}

int main(void)
{
    wnd.pos.x = 160;
    wnd.pos.y = 30;
    wnd.size.w = 480;
    wnd.size.h = 500;
    wnd.title = "plane war";
    int h,w;
    plane_x=130;
    plane_y = 440;
    background = malloc(480*1600*3);
    plane = malloc(45*273*3);
    bigEnemy = malloc(64*401*3);
    middleEnemy = malloc(46*311*3);
    smallEnemy = malloc(45*340*3);
    bullet1 = malloc(15*22*3);
    api_createwindow(&wnd);
    read24BitmapFile(filename, background, &h, &w);
    read24BitmapFile(heroplane, plane,&h,&w);
    read24BitmapFile(bigplane, bigEnemy, &h, &w);
    read24BitmapFile(middleplane, middleEnemy, &h, &w);
    read24BitmapFile(smallplane, smallEnemy, &h, &w);
    read24BitmapFile(bullet, bullet1, &h, &w);
    api_paint24BitmapToContentTransparent(&wnd, background, (Point){0,0},(Point){0,0},(Size){1600,480},(Size){500,480});
    api_paint24BitmapToContentTransparent(&wnd, plane, (Point){plane_x,plane_y}, (Point){0,0}, (Size){273,PLANE_WIDTH},(Size){PLANE_HEIGHT,PLANE_WIDTH});
    api_paint24BitmapToContentTransparent(&wnd, bigEnemy, (Point){0,0}, (Point){0,enemy_status[0]*enemyHeight[0]},(Size){401,64},(Size){enemyHeight[0],enemyWidth[0]});
    api_paint24BitmapToContentTransparent(&wnd, middleEnemy, (Point){50,0}, (Point){0,enemy_status[1]*enemyHeight[1]},(Size){311,46},(Size){enemyHeight[1],enemyWidth[1]});
    api_paint24BitmapToContentTransparent(&wnd, middleEnemy, (Point){100,20}, (Point){0,enemy_status[2]*enemyHeight[2]},(Size){311,46},(Size){enemyHeight[2],enemyWidth[2]});
    api_paint24BitmapToContentTransparent(&wnd, smallEnemy, (Point){150,40}, (Point){0,enemy_status[3]*enemyHeight[3]},(Size){340,45},(Size){enemyHeight[3],enemyWidth[3]});
    api_paint24BitmapToContentTransparent(&wnd, smallEnemy, (Point){200,20}, (Point){0,enemy_status[4]*enemyHeight[4]},(Size){340,45},(Size){enemyHeight[4],enemyWidth[4]});
    api_paint24BitmapToContentTransparent(&wnd, smallEnemy, (Point){250,40}, (Point){0,enemy_status[5]*enemyHeight[5]},(Size){340,45},(Size){enemyHeight[5],enemyWidth[5]});
    api_repaint(&wnd);
    api_settimer(&wnd, 50);
    api_exec(&wnd,&MsgProc);
    return 0;
}
