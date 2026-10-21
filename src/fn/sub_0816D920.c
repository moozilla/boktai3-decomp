#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct IconPos {
    s16 x, y, z, w;
};
void SceneHud_GetActionIconPosition(struct IconPos *out, s32 index)
{
    register s32 x asm("r0");
    s32 y = index;
    if (y <= 15) {
        s32 col = y & 3;
        s32 row = y >> 2;
        s32 a = col * 24 + 0x70;
        s32 b = row * 24 + 0x20;
        s32 z = 0;
        out->x = a;
        out->y = b;
        out->z = z;
        out->w = z;
        return;
    }
    if (y == 0x27) {
        x = 0xD8;
        y = 0x50;
        goto tail;
    }
    if (y == 0x26) {
        x = 0xD8;
        y = 0x38;
        goto tail;
    }
    if (y == 0x24 || y == 0x28) {
        x = 0xD8;
        y = 0x68;
        goto tail;
    }
    if (y == 0x20) {
        x = 0xD8;
    tail:
        out->x = x;
        out->y = y;
        out->z = 0;
        out->w = 0;
    } else {
        out->x = 0x90;
        out->y = 0x20;
    }
}
