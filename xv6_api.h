#ifndef XV6_API_H
#define XV6_API_H

#ifndef __ASSEMBLER__
struct RGB;
struct RGBA;
struct Rect;
struct Point;
struct Size;

#define USCREEN_WIDTH 800
#define USCREEN_HEIGHT 600
#define UTITLE_HEIGHT 30

typedef struct Rect {
    int x;
    int y;
    int h;
    int w;
} Rect;

typedef struct Point {
    int x;
    int y;
} Point;

typedef struct Size {
    int h;
    int w;
} Size;

typedef struct Window {
    int hwnd;
    Point pos;
    Size size;
    char* title;
    struct RGB* content;
    struct RGB* wholeContent;
} Window;

#endif
#endif
