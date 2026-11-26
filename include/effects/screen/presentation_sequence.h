#ifndef GUARD_PRESENTATION_SEQUENCE_H
#define GUARD_PRESENTATION_SEQUENCE_H

#include "engine/video/anim_sprite.h"
#include "engine/task.h"

struct PresentationSequence {
    struct Task task;
    struct AnimSpriteResource resource;
    struct AnimSprite layers[5];
    u8 unknown218[4];
    u16 option, dropTimer, timer;
    u8 unknown222[2];
    u16 frame, last, frameTimer, duration, result, exitTimer;
    void *handle;
    void (*callback)(void *);
};

void Presentation_UpdateDropShakeBlink(void *);

#endif
