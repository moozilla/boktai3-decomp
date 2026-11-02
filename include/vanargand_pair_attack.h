#ifndef GUARD_VANARGAND_PAIR_ATTACK_H
#define GUARD_VANARGAND_PAIR_ATTACK_H

#include "paired_scene_actor.h"
#include "scene_vec.h"
#include "vanargand_damage_part.h"

struct Player;
struct VanargandBodyAnimation {
    u8 unknown0[0x18];
    u16 flags;
    u8 unknown1A[6];
    struct SceneVec position;
    u8 unknown28[0x60 - 0x28];
};

struct VanargandPairAttack {
    u8 pad0[0x54];
    struct SceneVec position;
    u8 pad5C[0x90 - 0x5C];
    u32 actionTimer;
    s16 health;
    u8 pad96[6];
    u32 flags;
    u8 padA0[2];
    u8 phase;
    u8 action;
    u32 timer;
    u8 padA8[3], state, padAC[3];
    u8 finished;
    u8 entered;
    u8 pending;
    u8 animationEvent;
    u8 padB3[0x100 - 0xB3];
    void (*stateCallback)(void *, s32);
    u8 (*callback)(struct VanargandPairAttack *);
    u8 pad108[4];
    struct Player *player;
    u8 pad110[0x18C - 0x110];
    struct VanargandBodyAnimation animation;
    u8 drawable[0x21C - 0x1EC];
    u8 actionSequence[0x3FC - 0x21C];
    struct VanargandDamagePart part;
    struct MotionTrack track;
    struct VanargandAttackPair *pair;
    u8 pad570[0x580 - 0x570];
    u32 selected;
    u16 parameter584;
    u16 parameter;
    u16 parameter588;
    u16 drainDamage;
    u8 pad58C[0x59E - 0x58C];
    u16 thresholds[3];
    u32 hitFlags;
    s16 damage;
    u8 pad5AA[2];
    s16 accumulatedDamage;
    u16 flashTimer;
    u16 delay;
    s16 drainStartFrame;
    s16 repeats;
    u8 pad5B6[2];
    u16 attackParameter5B8, attackParameter5BA;
    u8 pad5BC[0x5C2 - 0x5BC];
    s16 mode;
    u8 unknown5C4[0x6C4 - 0x5C4];
    struct DamageNumberSource damageSource;
    u8 unknown6CC[0x718 - 0x6CC];
    u16 palettes[16][16];
    u8 fadeA[0xB18 - 0x918];
    u8 fadeB[0xD18 - 0xB18];
    u8 fadeC[0xF24 - 0xD18];
    u32 variant;
    u8 unknownF28[0xF34 - 0xF28];
    s32 transition;
    u8 unknownF38[4];
    u16 flash;
    u8 unknownF3E[6];
    void *afterimages;
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
