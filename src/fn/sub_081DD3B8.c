#include "global.h"
#define NameEntry_MoveCursorDown sub_081DD3B8
void NameEntry_UpdateCursor(void *);
void Sound_PlaySong(s32);
void NameEntry_MoveCursorDown(u8 *p)
{
    switch (*(u32 *)(p + 0x34c)) {
        case 0x23:
            *(u32 *)(p + 0x34c) = 0x0;
            break;
        case 0x24:
            *(u32 *)(p + 0x34c) = 0x1;
            break;
        case 0x25:
            *(u32 *)(p + 0x34c) = 0x2;
            break;
        case 0x26:
            *(u32 *)(p + 0x34c) = 0x3;
            break;
        case 0x27:
            *(u32 *)(p + 0x34c) = 0x4;
            break;
        case 0x4b:
            *(u32 *)(p + 0x34c) = 0x28;
            break;
        case 0x4c:
            *(u32 *)(p + 0x34c) = 0x29;
            break;
        case 0x4d:
            *(u32 *)(p + 0x34c) = 0x2a;
            break;
        case 0x4e:
            *(u32 *)(p + 0x34c) = 0x2b;
            break;
        case 0x4f:
            *(u32 *)(p + 0x34c) = 0x2c;
            break;
        case 0x50:
            *(u32 *)(p + 0x34c) = 0x52;
            break;
        case 0x51:
            *(u32 *)(p + 0x34c) = 0x53;
            break;
        case 0x52:
            *(u32 *)(p + 0x34c) = 0x54;
            break;
        case 0x53:
            *(u32 *)(p + 0x34c) = 0x54;
            break;
        case 0x54:
            *(u32 *)(p + 0x34c) = 0x55;
            break;
        case 0x55:
            *(u32 *)(p + 0x34c) = 0x56;
            break;
        case 0x56:
            *(u32 *)(p + 0x34c) = 0x50;
            break;
        default:
            if (*(u32 *)(p + 0x348) == 1 && *(u32 *)(p + 0x34c) == 0x4a)
                *(u32 *)(p + 0x34c) = 0x4e;
            else
                *(u32 *)(p + 0x34c) += 5;
            break;
    }
    NameEntry_UpdateCursor(p);
    Sound_PlaySong(0xdc);
}
