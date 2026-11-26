#include "global.h"

struct NamedObjectChase {
    u8 pad0[0x58];
    u8 step[8];
    u8 pad60[0xB6 - 0x60];
    u16 state;
    u8 padB8[2];
    u8 entered;
    u8 padBB;
    u8 padBC[2];
    s16 near;
    u8 padC0[0xF4 - 0xC0];
    u8 target[0x0C];
    u8 body[1];
};
u32 NamedObject_StepTowardTarget(void *, void *, void *, s32, s32, s32);
void NamedObject_PlayAdjacentExitSound(void *);
void NamedObject_EmitMovementTrail(void *);

void sub_081012EC(struct NamedObjectChase *o)
{
    if ((u8)NamedObject_StepTowardTarget(o->body, o->target, o->step, o->near, o->near, o->near)) {
        o->state = 3;
        o->entered = 1;
        NamedObject_PlayAdjacentExitSound(o);
    } else {
        NamedObject_EmitMovementTrail(o);
    }
}
