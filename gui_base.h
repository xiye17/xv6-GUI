#ifndef GUI_BASE_H
#define GUI_BASE_H

#define GUI_BUF 0x9000

#ifndef __ASSEMBLER__

ushort SCREEN_WIDTH;
ushort SCREEN_HEIGHT;
int screen_size;

// 24 bit RGB. used in GUI Utility
typedef struct RGB {
    unsigned char B;
    unsigned char G;
    unsigned char R;
} RGB;

// 32 bit RGBA. used above GUI Utility
typedef struct RGBA {
    unsigned char A;
    unsigned char B;
    unsigned char G;
    unsigned char R;
} RGBA;

#endif
#endif
