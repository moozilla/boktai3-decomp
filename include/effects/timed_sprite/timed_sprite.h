#ifndef GUARD_TIMED_SPRITE_H
#define GUARD_TIMED_SPRITE_H

#include "engine/shared/contact_collider.h"
#include "engine/shared/display_sequence.h"
#include "engine/interrupts_and_video/sprite.h"
#include "engine/shared/task.h"

struct TimedSprite {
    struct DisplaySequenceSprite sprite;
    struct DisplaySequenceState sequence;
    struct ContactPosition velocity;
    struct ContactCollider collider;
    u16 age;
    u8 state;
    s8 slot;
    u16 blinkAge;
    u8 enter, angle;
};

struct TimedSpritePool {
    struct Task task;
    struct SpriteRenderResource graphics;
    void *sequenceData;
    u32 allocated;
    struct TimedSprite sprites[12];
};

#endif
