/* Provisional names and object interpretation; offsets follow static accesses. */
#include "global.h"
#define StageResults_InitCursorPositions sub_08111E88
struct E {
    u8 pad[0x44];
    u16 x, y;
    u8 pad2[0x60 - 0x48];
};
void StageResults_InitCursorPositions(struct E *entries)
{
    s32 i;
    for (i = 0; i <= 22; i++) {
        switch (i) {
            case 8: {
                struct E *e = entries + i;
                e->x = 0xb0;
                e->y = 0x18;
            } break;
            case 9: {
                struct E *e = entries + i;
                e->x = 0xb8;
                e->y = 0x18;
            } break;
            case 10: {
                struct E *e = entries + i;
                e->x = 0xc0;
                e->y = 0x18;
            } break;
            case 6: {
                struct E *e = entries + i;
                e->x = 0xc8;
                e->y = 0x18;
            } break;
            case 11: {
                struct E *e = entries + i;
                e->x = 0xd0;
                e->y = 0x18;
            } break;
            case 12: {
                struct E *e = entries + i;
                e->x = 0xd8;
                e->y = 0x18;
            } break;
            case 13: {
                struct E *e = entries + i;
                e->x = 0xe0;
                e->y = 0x18;
            } break;
            case 14: {
                struct E *e = entries + i;
                e->x = 0xd0;
                e->y = 0x50;
            } break;
            case 15: {
                struct E *e = entries + i;
                e->x = 0xd8;
                e->y = 0x50;
            } break;
            case 16: {
                struct E *e = entries + i;
                e->x = 0xe0;
                e->y = 0x50;
            } break;
            case 17: {
                struct E *e = entries + i;
                e->x = 0xd0;
                e->y = 0x68;
            } break;
            case 18: {
                struct E *e = entries + i;
                e->x = 0xd8;
                e->y = 0x68;
            } break;
            case 19: {
                struct E *e = entries + i;
                e->x = 0xe0;
                e->y = 0x68;
            } break;
            case 20: {
                struct E *e = entries + i;
                e->x = 0xd0;
                e->y = 0x88;
            } break;
            case 21: {
                struct E *e = entries + i;
                e->x = 0xd8;
                e->y = 0x88;
            } break;
            case 22: {
                struct E *e = entries + i;
                e->x = 0xe0;
                e->y = 0x88;
            } break;
            default: {
                struct E *e = entries + i;
                e->x = 0;
                e->y = 0;
            } break;
        }
    }
}
