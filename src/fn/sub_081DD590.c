#include "global.h"
#define NameEntry_MoveCursorRight sub_081DD590
void NameEntry_UpdateCursor(void *);
void Sound_PlaySong(s32);
void NameEntry_MoveCursorRight(u8 *p)
{
    switch (*(u32 *)(p + 0x34c)) {
        case 0x4:
            *(u32 *)(p + 0x34c) = 0x28;
            break;
        case 0x9:
            *(u32 *)(p + 0x34c) = 0x2d;
            break;
        case 0xe:
            *(u32 *)(p + 0x34c) = 0x32;
            break;
        case 0x13:
            *(u32 *)(p + 0x34c) = 0x37;
            break;
        case 0x18:
            *(u32 *)(p + 0x34c) = 0x3c;
            break;
        case 0x1d:
            *(u32 *)(p + 0x34c) = 0x41;
            break;
        case 0x22:
            *(u32 *)(p + 0x34c) = 0x46;
            break;
        case 0x27:
            *(u32 *)(p + 0x34c) = 0x4b;
            break;
        case 0x2c:
            *(u32 *)(p + 0x34c) = 0x50;
            break;
        case 0x31:
            *(u32 *)(p + 0x34c) = 0x52;
            break;
        case 0x36:
            *(u32 *)(p + 0x34c) = 0x54;
            break;
        case 0x3b:
            *(u32 *)(p + 0x34c) = 0x55;
            break;
        case 0x40:
            *(u32 *)(p + 0x34c) = 0x56;
            break;
        case 0x45:
            *(u32 *)(p + 0x34c) = 0x56;
            break;
        case 0x4a:
            *(u32 *)(p + 0x34c) = 0x56;
            break;
        case 0x4f:
            *(u32 *)(p + 0x34c) = 0x56;
            break;
        case 0x51:
            *(u32 *)(p + 0x34c) = 0x0;
            break;
        case 0x53:
            *(u32 *)(p + 0x34c) = 0x5;
            break;
        case 0x54:
            *(u32 *)(p + 0x34c) = 0xa;
            break;
        case 0x55:
            *(u32 *)(p + 0x34c) = 0xf;
            break;
        case 0x56:
            *(u32 *)(p + 0x34c) = 0x19;
            break;
        default:
            if (*(u32 *)(p + 0x348) == 1 && *(u32 *)(p + 0x34c) == 0x4e)
                *(u32 *)(p + 0x34c) = 0x4a;
            else
                *(u32 *)(p + 0x34c) += 1;
            break;
    }
    NameEntry_UpdateCursor(p);
    Sound_PlaySong(0xdc);
}
