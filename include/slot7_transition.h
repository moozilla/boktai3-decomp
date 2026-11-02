#ifndef GUARD_SLOT7_TRANSITION_H
#define GUARD_SLOT7_TRANSITION_H

#include "global.h"

struct Slot7Transition {
    u8 unk0[0x90];
    u32 callbackTimer;
    u8 unk94[0xA2 - 0x94];
    u8 phase, unkA3;
    u32 phaseTimer;
    u8 unkA8[7];
    u8 actionFlags[2]; /* finished, then entry pending */
    u8 callbackEntered;
    u8 unkB2[0xC4 - 0xB2];
    u32 actionTimer;
    u8 unkC8[0x100 - 0xC8];
    void (*callback)(void);
};

static inline u8 Slot7_TakeActionEntry(struct Slot7Transition *actor)
{
    u8 *enter = &actor->actionFlags[1];
    if (*enter) {
        *enter = 0;
        enter[-1] = 0;
        return TRUE;
    }
    return FALSE;
}

static inline void Slot7_TransitionTo(struct Slot7Transition *actor, void (*callback)(void))
{
    actor->callback = callback;
    actor->callbackTimer = 0;
    actor->callbackEntered = 1;
    actor->phase = 0;
    actor->phaseTimer = 0;
}

#endif
