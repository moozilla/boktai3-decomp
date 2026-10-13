#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Vec {
    s16 x, y, z, pad;
};
struct Sprite {
    u8 pad[28];
    struct Vec position;
};
struct Sprite *BreakableBarrier_SetPosition(struct Sprite *input, struct Vec *position,
                                            int orientation)
{
    register struct Sprite *temp asm("r3") = input;
    register struct Sprite *p asm("r4");
    register u32 x asm("r5");
    register u32 y asm("r3");
    register u32 value asm("r1");
    asm("" : : "r"(temp));
    p = temp;
    x = (u16)position->x;
    p->position.x = x;
    y = (u16)position->y;
    p->position.y = y;
    value = (u16)position->z;
    p->position.z = value;
    if (orientation) {
        value -= 128;
        p->position.z = value;
    } else {
        value = x - 128;
        p->position.x = value;
    }
    value = (u16)p->position.x + 60;
    p->position.x = value;
    value = (u16)p->position.z + 60;
    p->position.z = value;
    value = (u16)p->position.y + 20;
    p->position.y = value;
    return input;
}
