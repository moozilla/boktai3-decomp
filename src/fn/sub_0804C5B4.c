#include "tether_actor.h"
// MATCHING_CONCESSION: register-pinning
void TetherActor_SetEntry(struct TetherActor *, u32);
void TetherActor_SetState(struct TetherActor *, u32);
void DisplayAnimation_SetFlags(struct DisplayAnimation *, u32);
void DisplayAnimation_ClearFlags(struct DisplayAnimation *, u32);
s32 DisplayAnimation_SetPalette(struct DisplayAnimation *, u32, u32, u32);
#define TetherActor_UpdateDisplayDeparture sub_0804C5B4
void TetherActor_UpdateDisplayDeparture(struct TetherActor *actor)
{
    u16 *frame;
    register u32 current asm("r0");
    actor->heading = 0;
    actor->chargeCollider.flags |= 4;
    frame = &actor->stateFrame;
    ++*frame;
    TetherActor_SetEntry(actor, 0);
    current = *frame;
    asm("" : "+r"(current) : : "memory");
    if (current <= 27)
        return;
    if (current <= 43) {
        u32 savedFrame = current;
        s32 phase = savedFrame - 28;
        struct DisplayAnimation *display = &actor->display.animation;
        s32 first, second;
        DisplayAnimation_SetFlags(display, 2);
        first = 64 - phase * 4;
        second = 64;
        if (phase > 8) {
            second = phase * 8;
            {
                u32 height = (savedFrame - 36) * 30;
                actor->displayOffsetY = height;
            }
        }
        DisplayAnimation_SetPalette(display, first, second, 0);
    } else if (current <= 51)
        actor->displayOffsetY = (*frame - 44) * 200 + 240;
    else
        DisplayAnimation_SetFlags(&actor->display.animation, 1);
}
