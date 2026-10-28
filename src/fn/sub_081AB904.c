#include "global.h"
#include "input.h"

// confidence=low: cycles five auxiliary-animation debug modes on a key press.
#define Vanargand_UpdatePartDebugControls sub_081AB904
struct DebugPart {
    u8 pad0[0x10];
    u16 mode;
    u8 pad12[0x48 - 0x12];
    u8 animation[0x10];
    void *resource;
    u32 counter;
    u8 pad60[0x104 - 0x60];
    u16 active;
};
struct DebugPartActor {
    u8 pad0[0xA2];
    u8 phase;
    u8 padA3;
    u32 timer;
    u8 padA8[9];
    u8 pending;
    u8 padB2[0x3FC - 0xB2];
    struct DebugPart part;
};
void Debug_NudgeWorldPositionWithA(void *);
void Debug_NudgeWorldPositionWithB(void *);
void Anim_SetSequence(void *, void *, s32, s32, s32);
void Vanargand_UpdatePartDebugControls(struct DebugPartActor *p)
{
    struct DebugPart *part = &p->part;
    if (p->pending) {
        p->pending = 0;
        p->phase = 0;
        p->timer = 0;
    }
    Debug_NudgeWorldPositionWithA((u8 *)p + 0x418);
    Debug_NudgeWorldPositionWithB((u8 *)p + 0x478);
    if (gPadState[1] & 0x100) {
        p->phase = (p->phase + 1) % 5;
        p->timer = 0;
    }
    switch (p->phase) {
        case 0:
            if (p->timer == 0) {
                part->mode = 7;
                part->active = 0;
                p->timer++;
            }
            break;
        case 1:
            if (p->timer == 0) {
                Anim_SetSequence(part->animation, part->resource, 0, 0, 0);
                part->active = 1;
                part->counter = 0;
                p->timer++;
            }
            break;
        case 2:
            if (p->timer == 0) {
                u32 mode = 4;
                Anim_SetSequence(part->animation, part->resource, 0, 0, mode);
                part->active = 1;
                part->counter = 0;
                p->timer++;
            } else {
                break;
            }
        case 3:
            if (p->timer == 0) {
                u32 one = 1;
                Anim_SetSequence(part->animation, part->resource, one, 0, 0);
                part->active = one;
                part->counter = 0;
                p->timer++;
            }
            break;
        case 4:
            if (p->timer == 0) {
                Anim_SetSequence(part->animation, part->resource, 2, 0, 0);
                part->active = 1;
                part->counter = 0;
                p->timer++;
            }
            break;
    }
}
