#include "breakable_barrier.h"
// MATCHING_CONCESSION: register-pinning
struct BreakableBarrier *BreakableBarrier_SetPosition(struct BreakableBarrier *input,
                                                      struct SceneVec *position, int orientation)
{
    register struct BreakableBarrier *temp asm("r3") = input;
    register struct BreakableBarrier *p asm("r4");
    register u32 x asm("r5");
    register u32 y asm("r3");
    register u32 value asm("r1");
    asm("" : : "r"(temp));
    p = temp;
    x = (u16)position->x;
    p->sprite.position.x = x;
    y = (u16)position->y;
    p->sprite.position.y = y;
    value = (u16)position->z;
    p->sprite.position.z = value;
    if (orientation) {
        value -= 128;
        p->sprite.position.z = value;
    } else {
        value = x - 128;
        p->sprite.position.x = value;
    }
    value = (u16)p->sprite.position.x + 60;
    p->sprite.position.x = value;
    value = (u16)p->sprite.position.z + 60;
    p->sprite.position.z = value;
    value = (u16)p->sprite.position.y + 20;
    p->sprite.position.y = value;
    return input;
}
