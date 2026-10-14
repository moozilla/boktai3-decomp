#ifndef GUARD_CONTACT_SEQUENCE_ACTOR_H
#define GUARD_CONTACT_SEQUENCE_ACTOR_H
#include "contact_swarm.h"
/* Tentative family name: eight sequence actors, each with health and resource colliders. */
struct ContactSequenceActor {
    u8 active, state, enter, variant;
    u16 flags;
    u8 cooldown, flash;
    s16 health;
    u16 reactionValue;
    u32 stateFrame;
    struct ContactPosition position;
    struct ContactCollider resourceCollider, healthCollider;
    struct ContactSwarmGraphics graphics;
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState sequence;
};
struct ContactSequenceOwner {
    u8 task[0x18];
    u32 configuration;
    u32 contactValue;
    s32 initialHealth;
    u32 reactionValue;
    u32 script;
    u32 activeCount;
    const void *sequenceResource;
    struct ContactSequenceActor actors[8]; /* 0x34, stride 0x118 */
    s16 fadeReverse, fadeLevel;
    u8 palette[0x20]; /* 0x8F8; owner size 0x918 */
};
#endif
