// MATCHING_CONCESSION: register-pinning
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

#define TRUNCATE_QUARTER(out, input)                                                               \
    do {                                                                                           \
        s16 value = (input);                                                                       \
        if (value >= 0)                                                                            \
            (out) = value >> 2;                                                                    \
        else                                                                                       \
            (out) = -(-value >> 2);                                                                \
    } while (0)

static inline s32 HealthThreshold(u16 health, s16 original)
{
    s32 quarter;
    TRUNCATE_QUARTER(quarter, health);
    return original - quarter;
}

void Axe_InitAttackCooldowns(struct AxeCooldownActor *actor)
{
    struct AxeCooldownState *state = actor->state;
    s32 i;
    int zero;
    s16 *healthField;
    if (Script_SeekToKeyword(0x50) != 0) {
        for (i = 0; i <= 3; i++) {
            state->lengths[i] = Script_GetPc() ? Script_GetValue() : 0;
        }
    }
    healthField = &actor->health;
    zero = 0;
    for (i = 3; i >= 0; i--)
        state->timers[i] = zero;
    {
        s32 health = *healthField;
        state->healthThreshold = HealthThreshold(health, *healthField);
    }
}
