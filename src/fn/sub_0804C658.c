#include "tether_actor.h"
// MATCHING_CONCESSION: register-pinning
void TetherActor_SetEntry(struct TetherActor *, u32);
void TetherActor_SetState(struct TetherActor *, u32);
void DisplayAnimation_SetFlags(struct DisplayAnimation *, u32);
void DisplayAnimation_ClearFlags(struct DisplayAnimation *, u32);
s32 DisplayAnimation_SetPalette(struct DisplayAnimation *, u32, u32, u32);
#define TetherActor_UpdateDisplayReturn sub_0804C658
void TetherActor_UpdateDisplayReturn(struct TetherActor *actor)
{
    u32 zero;
    u8 *heading = &actor->heading;
    u16 *frame;
    u32 current;
    zero = 0;
    *heading = zero;
    actor->chargeCollider.flags |= 4;
    TetherActor_SetEntry(actor, 0);
    frame = &actor->stateFrame;
    current = *frame;
    asm("" : "+r"(current) : : "memory");
    if (current <= 75)
        DisplayAnimation_SetFlags(&actor->display.animation, 1);
    else if (current <= 83) {
        u32 phase = *frame - 76;
        struct DisplayAnimation *display = &actor->display.animation;
        DisplayAnimation_ClearFlags(display, 1);
        DisplayAnimation_SetFlags(display, 2);
        DisplayAnimation_SetPalette(display, 4, 120, 0);
        actor->displayOffsetY = (8 - phase) * 200 + 240;
    } else if (current <= 99) {
        s32 phase = *frame - 84;
        s32 remaining = 15 - phase;
        s32 first = 64 - remaining * 4, second;
        if (phase <= 7) {
            second = remaining * 8;
            actor->displayOffsetY = (8 - phase) * 30;
        } else {
            second = 64;
            actor->displayOffsetY = zero;
        }
        DisplayAnimation_SetPalette(&actor->display.animation, first, second, 0);
    }
    if (++actor->stateFrame > 103) {
        DisplayAnimation_ClearFlags(&actor->display.animation, 2);
        TetherActor_SetState(actor, 0);
    }
}
