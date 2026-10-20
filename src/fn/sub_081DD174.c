#include "global.h"
#define NameEntry_MoveCursorUp sub_081DD174
void NameEntry_UpdateCursor(void *);
void Sound_PlaySong(s32);
void NameEntry_MoveCursorUp(u8 *p)
{
    switch (*(u32 *)(p + 0x34c)) {
        case 0:
            *(u32 *)(p + 0x34c) = 0x23;
            break;
        case 1:
            *(u32 *)(p + 0x34c) = 0x24;
            break;
        case 2:
            *(u32 *)(p + 0x34c) = 0x25;
            break;
        case 3:
            *(u32 *)(p + 0x34c) = 0x26;
            break;
        case 4:
            *(u32 *)(p + 0x34c) = 0x27;
            break;
        case 40:
            *(u32 *)(p + 0x34c) = 0x4b;
            break;
        case 41:
            *(u32 *)(p + 0x34c) = 0x4c;
            break;
        case 42:
            *(u32 *)(p + 0x34c) = 0x4d;
            break;
        case 43:
            *(u32 *)(p + 0x34c) = 0x4e;
            break;
        case 44:
            *(u32 *)(p + 0x34c) = 0x4f;
            break;
        case 80:
            *(u32 *)(p + 0x34c) = 0x56;
            break;
        case 81:
            *(u32 *)(p + 0x34c) = 0x56;
            break;
        case 82:
            *(u32 *)(p + 0x34c) = 0x50;
            break;
        case 83:
            *(u32 *)(p + 0x34c) = 0x51;
            break;
        case 84:
            *(u32 *)(p + 0x34c) = 0x52;
            break;
        case 85:
            *(u32 *)(p + 0x34c) = 0x54;
            break;
        case 86:
            *(u32 *)(p + 0x34c) = 0x55;
            break;
        default:
            *(u32 *)(p + 0x34c) -= 5;
            break;
    }
    NameEntry_UpdateCursor(p);
    Sound_PlaySong(0xdc);
}
