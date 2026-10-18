#ifndef GUARD_ACTOR_STATS_H
#define GUARD_ACTOR_STATS_H
#include "global.h"
/* Tentative stat-bearing actor prefix; 0811C5F8 and 0811D0C4 share counters. */
struct ActorStats {
    u8 unknown0[0x44];
    u8 state44[4], state48[0x10];
    s16 value, limit;
    s32 amount, amountLimit;
    u8 unknown64[4];
    s32 pendingDamage;
    u8 unknown6C[0x18];
    s32 rate84, limit88;
    u8 unknown8C[4];
    s32 parameter90, parameter94, scaledParameter98;
    s32 parameter9C, parameterA0, parameterA4, parameterA8;
    u8 unknownAC[0x28A];
    u16 factor336;
    u8 unknown338[0x1E1];
    u8 rate519, factor51A;
    u8 unknown51B[0xD];
    u16 increment;
    u8 mode, pending;
    u8 unknown52C[0x4E];
    u8 damageTimer;
    u8 unknown57B[0x73];
    u8 damageFlags;
};
#endif
