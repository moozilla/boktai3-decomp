#ifndef GUARD_MOVING_GLYPH_H
#define GUARD_MOVING_GLYPH_H

#include "global.h"
#include "engine/task.h"
#include "engine/video/sprite.h"

struct MovingSprite {
    struct Task task;
    struct DisplaySequenceSprite sprite;
    u8 animation[0x1C];
    struct ContactPosition position;
    u8 unknown68[2];
    u16 id;
    u16 angle;
    s16 velocityX, velocityY;
    u16 delay, duration, frame;
    void (*phase)(void);
};

#endif
