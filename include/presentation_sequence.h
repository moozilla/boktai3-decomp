#ifndef GUARD_PRESENTATION_SEQUENCE_H
#define GUARD_PRESENTATION_SEQUENCE_H

#include "scene_input_widget.h"

struct PresentationSequence {
    u8 task[0x18];
    struct SceneInputResource resource;
    struct SceneInputWidget layers[5];
    u8 unknown218[4];
    u16 option, dropTimer, timer;
    u8 unknown222[2];
    u16 frame, last, frameTimer, duration, result, exitTimer;
    void *handle;
    void (*callback)(void *);
};

void Presentation_UpdateDropShakeBlink(void *);

#endif
