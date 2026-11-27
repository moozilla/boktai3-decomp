#ifndef GUARD_INCLUDE_LINK_WIRELESS_WIRELESS_KNOCKBACK_ACTOR_H
#define GUARD_INCLUDE_LINK_WIRELESS_WIRELESS_KNOCKBACK_ACTOR_H

#include "global.h"

struct WirelessKnockbackActor {
    u8 pad0[0x68];
    s32 value; /* confidence=low */
    s32 type; /* confidence=low */
    u8 pad70[0x50];
    s32 unkC0; /* confidence=low */
    u8 padC4[0x274];
    u16 unk338; /* confidence=low */
    u8 pad33A[0x232];
    u32 f56C; /* confidence=low */
    u8 pad570[0xa];
    s8 unk57A; /* confidence=low */
    u8 pad57B; /* confidence=low */
    s16 posX; /* confidence=low */
    s16 posY;
    s16 posZ; /* confidence=low */
    u8 pad582[0xa];
    s16 offX; /* confidence=low */
    s16 offY;
    s16 offZ; /* confidence=low */
    u8 pad592[0x1a];
    s32 f5AC; /* confidence=low */
    u8 pad5B0[0x1c];
    s32 velX; /* confidence=low */
    u8 pad5D0[0x4];
    s32 velZ; /* confidence=low */
    u8 pad5D8[0x14];
    s8 unk5EC; /* confidence=low */
    u8 pad5ED; /* confidence=low */
    u8 flags;
    u8 pad5EF[0x1];
};

typedef char WirelessKnockbackActorSizeCheck[sizeof(struct WirelessKnockbackActor) == 0x5f0 ? 1 : -1];

#endif
