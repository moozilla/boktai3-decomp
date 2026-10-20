#include "global.h"
void sub_081DF1D8(void *);
void Sound_PlaySong(s32);
#define Password_MoveCursorRight sub_081DF878
void Password_MoveCursorRight(u8 *p)
{
    switch (*(u32 *)(p + 0x34c)) {
        case 4:
            *(u32 *)(p + 0x34c) = 30;
            break;
        case 9:
            *(u32 *)(p + 0x34c) = 35;
            break;
        case 14:
            *(u32 *)(p + 0x34c) = 40;
            break;
        case 19:
            *(u32 *)(p + 0x34c) = 45;
            break;
        case 24:
            *(u32 *)(p + 0x34c) = 50;
            break;
        case 29:
            *(u32 *)(p + 0x34c) = 55;
            break;
        case 34:
            *(u32 *)(p + 0x34c) = 60;
            break;
        case 39:
            *(u32 *)(p + 0x34c) = 62;
            break;
        case 44:
            *(u32 *)(p + 0x34c) = 64;
            break;
        case 49:
            *(u32 *)(p + 0x34c) = 65;
            break;
        case 54:
            *(u32 *)(p + 0x34c) = 65;
            break;
        case 59:
            *(u32 *)(p + 0x34c) = 65;
            break;
        case 61:
            *(u32 *)(p + 0x34c) = 0;
            break;
        case 63:
            *(u32 *)(p + 0x34c) = 5;
            break;
        case 64:
            *(u32 *)(p + 0x34c) = 63;
            break;
        case 65:
            *(u32 *)(p + 0x34c) = 15;
            break;
        default:
            *(u32 *)(p + 0x34c) += 1;
            break;
    }
    sub_081DF1D8(p);
    Sound_PlaySong(0xdc);
}
