#ifndef GUARD_TIMED_HAZARD_H
#define GUARD_TIMED_HAZARD_H

#include "engine/engine.h"
#include "engine/video/sprite.h"
#include "engine/video/display_sequence.h"
#include "scene/collision/contact_collider.h"
#include "scene/collision/contact_position.h"

// Tentative: a pool task owning a linked list of script-placed hazards that activate after a delay.

struct TimedHazard {
    u8 id, entered, state, flags; // flags: 1 body and attack colliders
    u8 animationDone, remove;
    u16 timer, delay;
    u8 unknownA[2];
    struct ContactPosition position;
    u16 ground;
    s16 health;
    struct ContactCollider *collider, *attack;
    u8 resource[0x1C];
    struct DisplaySequenceSprite sprite;
    struct DisplaySequenceState anim;
    struct TimedHazard *previous, *next;
};

struct TimedHazardPool {
    struct Task task;
    void *animation;
    struct TimedHazard *head;
};

s32 ColliderSprite_Destroy(struct TimedHazard *hazard);
u32 TimedHazard_LinkInstance(struct TimedHazardPool *pool, struct TimedHazard *hazard);
u32 TimedHazard_UnlinkInstance(struct TimedHazardPool *pool, struct TimedHazard *hazard);

#endif
