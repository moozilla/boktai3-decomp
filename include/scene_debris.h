#ifndef GUARD_SCENE_DEBRIS_H
#define GUARD_SCENE_DEBRIS_H

#include "contact_position.h"

s32 sub_08113398(const struct ContactPosition *position, const struct ContactPosition *velocity,
                 u32 spriteResource, u32 spriteGraphics, u32 sprite, u32 keyframe, u32 a6,
                 u32 a7, s32 palette, u32 timer, u32 motion, u32 a11, u32 a12, u32 restitution);

#endif
