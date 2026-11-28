#ifndef GUARD_CHARGED_BURST_H
#define GUARD_CHARGED_BURST_H

#include "global.h"
#include "engine/engine.h"
#include "scene/collision/contact_swarm.h"

struct ChargedBurstSprite {
    u32 flags;
    u8 unk_04[0xB];
    u8 unk_0F, unk_10;
    u8 unk_11[7];
    struct ContactPosition position;
    u8 unk_20[8];
};

struct ChargedBurstSpark {
    struct ChargedBurstSprite sprite;
    u8 active, timer;
    u8 unk_2A[2];
};

struct ChargedBurst {
    struct Task task;
    struct ContactPosition position;
    struct ContactCollider collider;
    struct ChargedBurstSprite core;
    void *coreResource;
    void *sparkResource;
    struct ChargedBurstSpark sparks[8];
    u8 nextSpark, sparkDelay;
    u16 unk_206, unk_208;
    u16 timer;
    void (*state)(struct ChargedBurst *);
};

void ChargedBurst_SetState(struct ChargedBurst *burst, void (*state)(struct ChargedBurst *));

#endif
