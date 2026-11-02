#include "global.h"
#include "engine.h"

// confidence=low: initializes scripted cooldown lengths and the health threshold.

struct AxeCooldownState {
    u8 filler[0xeb4];
    u16 healthThreshold;
    u16 pad;
    u16 timers[4];
    u16 lengths[4];
};

struct AxeCooldownActor {
    u8 filler[0x9c];
    s16 health;
    u8 filler2[0x3d0 - 0x9e];
    struct AxeCooldownState *state;
};

void Axe_InitAttackCooldowns(struct AxeCooldownActor *actor)
{
    struct AxeCooldownState *state = actor->state;
    s32 i;
    s32 health, quarter;
    s16 value;
    s16 *healthField;
    s32 zero;
    if (Script_SeekToKeyword(0x50) != 0) {
        for (i = 0; i <= 3; i++) {
            state->lengths[i] = Script_GetPc() ? Script_GetValue() : 0;
        }
    }
    healthField = &actor->health;
    zero = 0;
    for (i = 3;; --i) {
        if (i < 0) {
            quarter = *healthField;
            health = quarter;
            break;
        }
        state->timers[i] = zero;
    }
    value = quarter;
    if (value >= 0)
        quarter = value >> 2;
    else
        quarter = -(-value >> 2);
    state->healthThreshold = health - quarter;
}
