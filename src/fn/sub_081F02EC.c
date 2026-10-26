// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "game_state.h"

// foundry draft (worker best 95.9%); what differs is mostly register allocation and instruction order

struct Slot7Action {
    u8 pad0[0x3e];
    u16 phase;
    u8 pad40[3];
    u8 state;
    u16 cooldown;
};
struct Slot7Actor {
    u8 pad0[0x5c0];
    u16 duration;
    u16 flashTimer;
    u8 pad5c4[0x2c];
    s16 baseDuration;
};
void Collision_CalculateDamageResponse(void *);
s32 Div(s32, s32);

void SingletonSlot7_BeginScaledCooldown(void *event, struct Slot7Action *action,
                                        struct Slot7Actor *actor)
{
    s16 level;
    s32 duration;
    Collision_CalculateDamageResponse(event);
    if (action->cooldown) {
        action->phase = 0;
        action->state = 0;
    } else {
        level = gGameState->unk876;
        duration = level - 3;
        duration = Div(duration * 10, 6);
        if ((s32)duration <= 2)
            duration = (u16)actor->baseDuration;
        else if ((s32)duration <= 5) {
            register u32 off asm("r1") = 0x5F0;
            register u8 *q asm("r0");
            asm("" : "+r"(off));
            q = (u8 *)actor + off;
            duration = *(u16 *)q;
            duration += (s16)duration >> 2;
        } else {
            register u32 off asm("r1") = 0x5F0;
            register u8 *q asm("r0");
            u16 base;
            asm("" : "+r"(off));
            q = (u8 *)actor + off;
            base = *(u16 *)q;
            duration = base + ((s16)base >> 1);
        }
        actor->duration = duration;
        action->cooldown = 30;
        actor->flashTimer = 15;
    }
}
