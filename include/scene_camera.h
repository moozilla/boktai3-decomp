#ifndef GUARD_SCENE_CAMERA_H
#define GUARD_SCENE_CAMERA_H
#include "global.h"
/* Scene projection camera; separate from the render camera and its bounds task. */
struct SceneArcCamera {
    u8 unknown0[0x28];
    s32 viewportWidth; /* 0x28: used to scale participant meters */
    u8 unknown2C[0x18];
    s32 x;
};
#endif
