#ifndef __ASSEMBLER__

#define MAX_MSG_COUNT 50;
struct Rect;
struct Point;
struct Size;

typedf struct MsgQuene {
    int head;
    int tail;
    int length;
    Message msgList[MAX_MSG_COUNT];
} MsgQuene;

typedef struct WndInfo {
    int hwnd;
    Rect wndTitleBar;
    Rect wndContent;
    proc * procPtr;
    MsgQuene * msgQ;
} WndInfo;

#endif
