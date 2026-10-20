#include "global.h"
void sub_081DF1D8(void *);
void Sound_PlaySong(s32);
#define Password_MoveCursorDown sub_081DF510
void Password_MoveCursorDown(u8 *p)
{
    switch (*(u32 *)(p + 0x34c)) {
        case 25:
            *(u32 *)(p + 0x34c) = 0;
            break;
        case 26:
            *(u32 *)(p + 0x34c) = 1;
            break;
        case 27:
            *(u32 *)(p + 0x34c) = 2;
            break;
        case 28:
            *(u32 *)(p + 0x34c) = 3;
            break;
        case 29:
            *(u32 *)(p + 0x34c) = 4;
            break;
        case 55:
            *(u32 *)(p + 0x34c) = 30;
            break;
        case 56:
            *(u32 *)(p + 0x34c) = 31;
            break;
        case 57:
            *(u32 *)(p + 0x34c) = 32;
            break;
        case 58:
            *(u32 *)(p + 0x34c) = 33;
            break;
        case 59:
            *(u32 *)(p + 0x34c) = 34;
            break;
        case 60:
            *(u32 *)(p + 0x34c) = 62;
            break;
        case 61:
            *(u32 *)(p + 0x34c) = 63;
            break;
        case 62:
            *(u32 *)(p + 0x34c) = 64;
            break;
        case 63:
            *(u32 *)(p + 0x34c) = 64;
            break;
        case 64:
            *(u32 *)(p + 0x34c) = 65;
            break;
        case 65:
            *(u32 *)(p + 0x34c) = 60;
            break;
        default:
            *(u32 *)(p + 0x34c) += 5;
            break;
    }
    sub_081DF1D8(p);
    Sound_PlaySong(0xdc);
}
