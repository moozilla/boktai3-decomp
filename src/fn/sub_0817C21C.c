#include "global.h"
#define ActorState_UpdateTrigger sub_0817C21C

struct TriggerActor {
    u8 pad0[0xAA];
    u8 state;
    u8 padAB[3];
    u8 pendingValue;
    u8 pending;
    u8 trigger;
    u8 padB1[0x13];
    u32 ticks;
    u8 padC8[0x3AA];
    u8 action;
    u8 pad473[0x75];
    u8 active;
    u8 value;
    u8 pad4EA;
    u8 phase;
};
void ActorExtension_ResetState(struct TriggerActor *);
static inline u8 ConsumeTrigger(struct TriggerActor *p)
{
    if (p->trigger) {
        p->trigger = 0;
        p->pending = 0;
        return 1;
    }
    return 0;
}
void sub_0817C21C(struct TriggerActor *p)
{
    struct TriggerActor *actor = p;
    if (ConsumeTrigger(p)) {
        u32 state = 9;
        p->state = state;
        ActorExtension_ResetState(actor);
        actor->action = 0x70;
    }
    if (p->pending) {
        p->value = p->pendingValue;
        p->active = 1;
        p->phase = 0;
    }
    p->ticks++;
}
