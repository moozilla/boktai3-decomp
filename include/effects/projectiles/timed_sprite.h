#ifndef GUARD_TIMED_SPRITE_H
#define GUARD_TIMED_SPRITE_H

#include "scene/actor/contact_collider.h"
#include "engine/video/display_sequence.h"
#include "engine/video/sprite.h"
#include "engine/task.h"

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
