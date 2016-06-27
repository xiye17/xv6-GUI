
#define MAX_MSG_COUNT 50;

typeded struct Rect {
    int x;
    int y;
    int height;
    int width;
} Rect;

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
