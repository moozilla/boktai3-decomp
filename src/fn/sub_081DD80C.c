#include "global.h"
void NameEntry_UpdateCursor(void *);
void Sound_PlaySong(s32);
void NameEntry_MoveCursorLeft(u8 *p)
{
    switch (*(u32 *)(p + 0x34c)) {
        case 0x0:
            *(u32 *)(p + 0x34c) = 0x51;
            break;
        case 0x5:
            *(u32 *)(p + 0x34c) = 0x53;
            break;
        case 0xa:
            *(u32 *)(p + 0x34c) = 0x54;
            break;
        case 0xf:
            *(u32 *)(p + 0x34c) = 0x55;
            break;
        case 0x14:
            *(u32 *)(p + 0x34c) = 0x56;
            break;
        case 0x19:
            *(u32 *)(p + 0x34c) = 0x56;
            break;
        case 0x1e:
            *(u32 *)(p + 0x34c) = 0x56;
            break;
        case 0x23:
            *(u32 *)(p + 0x34c) = 0x56;
            break;
        case 0x28:
            *(u32 *)(p + 0x34c) = 0x4;
            break;
        case 0x2d:
            *(u32 *)(p + 0x34c) = 0x9;
            break;
        case 0x32:
            *(u32 *)(p + 0x34c) = 0xe;
            break;
        case 0x37:
            *(u32 *)(p + 0x34c) = 0x13;
            break;
        case 0x3c:
            *(u32 *)(p + 0x34c) = 0x18;
            break;
        case 0x41:
            *(u32 *)(p + 0x34c) = 0x1d;
            break;
        case 0x46:
            *(u32 *)(p + 0x34c) = 0x22;
            break;
        case 0x4b:
            *(u32 *)(p + 0x34c) = 0x27;
            break;
        case 0x50:
            *(u32 *)(p + 0x34c) = 0x2c;
            break;
        case 0x52:
            *(u32 *)(p + 0x34c) = 0x31;
            break;
        case 0x54:
            *(u32 *)(p + 0x34c) = 0x36;
            break;
        case 0x55:
            *(u32 *)(p + 0x34c) = 0x3b;
            break;
        case 0x56:
            *(u32 *)(p + 0x34c) = 0x45;
            break;
        default:
            *(u32 *)(p + 0x34c) -= 1;
            break;
    }
    NameEntry_UpdateCursor(p);
    Sound_PlaySong(0xdc);
}
