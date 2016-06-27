
#define MAX_MSG_COUNT 50;

struct Rect {
    int x;
    int y;
    int height;
    int width;
}

struct msgQuene {
    int head;
    int tail;
    int length;
    msg msgList[MAX_MSG_COUNT];
}

struct wndInfo {
    int hwnd;
    Rect wndTitleBar;
    Rect wndContent;
    proc * procPtr;
    msgQuene * msgQ;
}
