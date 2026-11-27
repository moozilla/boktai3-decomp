#ifndef GUARD_INCLUDE_SCENE_CAMERA_SCENE_CAMERA_VECTOR_LAYOUT_H
#define GUARD_INCLUDE_SCENE_CAMERA_SCENE_CAMERA_VECTOR_LAYOUT_H

#include "global.h"

struct SceneCameraVector {
    u16 x;
    u16 y; /* confidence=low */
    u16 z; /* confidence=low */
    u16 w; /* confidence=low */
} __attribute__((aligned(4)));

typedef char SceneCameraVectorSizeCheck[sizeof(struct SceneCameraVector) == 0x8 ? 1 : -1];

#endif
