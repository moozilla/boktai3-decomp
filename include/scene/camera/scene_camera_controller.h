#ifndef GUARD_SCENE_CAMERA_CONTROLLER_H
#define GUARD_SCENE_CAMERA_CONTROLLER_H

#include "engine/task.h"
#include "engine/video/render.h"

/* Task allocated at 082275D8. Both position paths share the scene offsets. */
struct SceneCameraController {
    struct Task task;
    u32 id;
    u16 moving, completionMode;
    u8 receiver[0x34];
    u16 secondaryMode, secondaryTimer;
    struct CameraPosition saved, target;
    u16 positionMode, positionTimer;
    struct CameraPosition position;
    s16 offsetX, offsetY, offsetZ;
    u16 unknown7A, targetId;
    u16 field7E, field80, field82;
    u8 field84, field85;
    u16 field86, field88, unknown8A;
    u16 boundsMode;
    /* X and Z in centered mode, difference and sum in diagonal mode. */
    s16 minBounds[2], maxBounds[2];
    u16 rateIsValue;
    s32 rate;
    struct CameraPosition *source;
    u32 *enabled;
};

typedef char SceneCameraControllerSizeCheck[sizeof(struct SceneCameraController) == 0xA4 ? 1 : -1];

#endif
