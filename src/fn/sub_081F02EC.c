
#ifdef NONMATCHING
// fuzzy: 95.9%
// foundry draft (worker best 95.9%); what differs is mostly register allocation and instruction order
#define SingletonSlot7_BeginScaledCooldown sub_081F02EC
#include "global.h"
#include "game_state.h"

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
void sub_0821FF8C(void *);
s32 Div(s32, s32);

void sub_081F02EC(void *event, struct Slot7Action *action, struct Slot7Actor *actor)
{
    s16 level;
    s32 duration;
    sub_0821FF8C(event);
    if (action->cooldown) {
        action->phase = 0;
        action->state = 0;
    } else {
        level = gUnk_02000710->unk876;
        duration = level - 3;
        duration = Div(duration * 10, 6);
        if ((s32)duration <= 2)
            duration = (u16)actor->baseDuration;
        else if ((s32)duration <= 5) {
            duration = (u16)actor->baseDuration;
            duration += (s16)duration >> 2;
        } else
            duration = (u16)actor->baseDuration + (actor->baseDuration >> 1);
        actor->duration = duration;
        action->cooldown = 30;
        actor->flashTimer = 15;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081F02EC);
#endif
