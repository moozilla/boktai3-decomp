#ifndef GUARD_VANARGAND_PAIR_ATTACK_H
#define GUARD_VANARGAND_PAIR_ATTACK_H

#include "paired_scene_actor.h"

struct VanargandPairAttack {
    u8 pad0[0x9C];
    u32 flags;
    u8 padA0[2];
    u8 phase;
    u8 padA3;
    u32 timer;
    u8 padA8[7];
    u8 finished;
    u8 entered;
    u8 pending;
    u8 padB2[0x104 - 0xB2];
    void *callback;
    u8 pad108[0x18C - 0x108];
    u8 animation[0x60];
    u8 drawable[0x56C - 0x1EC];
    struct VanargandAttackPair *pair;
    u8 pad570[0x580 - 0x570];
    u32 selected;
    u16 parameter584;
    u16 parameter;
    u16 parameter588;
    u8 pad58A[0x5B0 - 0x58A];
    u16 delay;
    u8 pad5B2[2];
    s16 repeats;
    u8 pad5B6[0x5C2 - 0x5B6];
    s16 mode;
    u8 unknown5C4[0xF3C - 0x5C4];
    u16 flash;
};

static inline struct VanargandAttackPair *Vanargand_GetAttackPair(struct VanargandPairAttack *owner)
{
    return owner->pair;
}

static inline u32 Vanargand_HalfState(const struct VanargandAttackPair *pair, u32 half)
{
    return half ? pair->halves[1].requestedState : pair->halves[0].requestedState;
}

#endif
