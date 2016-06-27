#ifndef __ASSEMBLER__

#define MAX_MSG_COUNT 50
#define MAX_WINDOW_COUNT 10

struct Rect;
struct Point;
struct Size;
struct message;
struct proc;

typedef struct MsgQueue {
    int head;
    int tail;
    int length;
    struct message msgList[MAX_MSG_COUNT];
} MsgQueue;

typedef struct WndInfo {
    int hwnd;
    Rect wndTitleBar;
    Rect wndBody;
    struct proc * procPtr;
    struct RGB * content;
    MsgQueue msgQ;
} WndInfo;

extern struct WndInfo wndInfoList[MAX_WINDOW_COUNT];
extern int wndCount;

typedef struct Focus {
    int x;
    int y;
    int ID;
} Focus;

typedef struct MousePos {
    int x;
    int y;
} MousePos;

#endif
