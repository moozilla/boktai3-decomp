#ifndef GUARD_SCALED_SHADOW_TYPES_H
#define GUARD_SCALED_SHADOW_TYPES_H
#include "terrain_shadow.h"

struct ScaledShadowSprite {
    u32 flags;
    u8 queued, enabled, rotation, priority;
    u8 scaleX, scaleY, unknownA[2];
    const void *resource;
    u16 frame;
    u8 frameIndex, elapsed, width, height, originX, originY;
    u32 attributes;
    struct SceneVec position;
    struct ScaledShadowSprite *previous, *next;
};
struct ShadowAnimationResource {
    u8 flags, unknown1;
    u16 unknown2, unknown4, palette;
    u8 unknown8[4];
    void *paletteData, *tileData, *frames;
    u32 unknown18;
};
struct ScaledShadow {
    u8 active, flags;
    u16 unknown2, unknown4, unknown6;
    struct SceneVec *source;
    struct ScaledShadowSprite sprite;
    struct ShadowAnimationResource resource;
    s8 scale;
    u8 minScale, maxScale, unknown57;
    s16 maxHeight, minHeight;
    u16 shift, nearShift;
    void (*update)(struct ScaledShadow *);
    struct ScaledShadow *previous, *next;
};
struct ScaledShadowManager {
    struct Task task;
    u32 unknown18;
    struct ScaledShadow *head;
};
#endif
