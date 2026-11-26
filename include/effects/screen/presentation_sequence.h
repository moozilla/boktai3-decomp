#ifndef GUARD_PRESENTATION_SEQUENCE_H
#define GUARD_PRESENTATION_SEQUENCE_H

#include "ui/widgets/scene_input_widget.h"
#include "engine/task.h"

struct PresentationSequence {
    struct Task task;
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
