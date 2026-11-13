#ifndef GUARD_ACTOR_STATS_H
#define GUARD_ACTOR_STATS_H
#include "global.h"
#include "actor_ability_types.h"
struct ActorDrainState {
    void (*update)(void);
    void *displayPosition;
    u16 counter, amount;
    u8 unknownC, mode, enabled, phase;
};
/* Tentative stat-bearing actor prefix; 0811C5F8 and 0811D0C4 share counters. */
struct ActorStats {
    u8 unknown0[0x10];
    u8 position[0x28];
    s32 height;
    u8 unknown3C[8];
    u8 state44[4], state48[0x10];
    s16 value, limit;
    s32 amount, amountLimit;
    u8 unknown64[4];
    s32 pendingDamage;
    u8 unknown6C[0x12];
    u16 recoveryValue;
    u8 unknown80[4];
    s32 rate84, limit88;
    u8 unknown8C[4];
    s32 parameter90, parameter94, scaledParameter98;
    s32 parameter9C, parameterA0, parameterA4, parameterA8;
    u8 unknownAC[4];
    struct ActorDrainState drain;
    u8 unknownC0[0x254];
    struct ActorAbility ability;
    void (*secondaryCallback)(void); /* Third-source record starts at 0x330. */
    u16 secondaryId;
    u16 factor336;
    u16 secondaryConfig[2];
    s32 secondaryKind;
    u8 secondaryEnabled, unknown341[3];
    u32 secondaryArgument;
    u8 unknown348[0x1D0];
    u8 sourceInfoId;
    u8 rate519, factor51A;
    u8 unknown51B[0xD];
    u16 increment;
    u8 mode, pending;
    u8 collisionBody[0x40];
    u8 contactHeader[0xE];
    u8 damageTimer;
    u8 unknown57B[0x41];
    s32 relativeOrigin[4];
    u8 unknown5CC[0x22];
    u8 damageFlags;
};
#endif
