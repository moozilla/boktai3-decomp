#ifndef GUARD_SPRITE_TRAIL_H
#define GUARD_SPRITE_TRAIL_H

#include "engine/interrupts_and_video/sprite.h"
#include "engine/shared/task.h"

struct TrailVector {
    s32 x, y, z, pad;
};

struct SpriteTrail {
    u8 entered, state;
    u8 unknown2[2];
    struct TrailVector position, history[4], velocity, target;
    u32 style;
    s32 collisionArg0, collisionArg1;
    s32 historyIndex;
    u32 frame;
    u8 collider[0x40], renderCollider[0x40];
    struct DisplaySequenceSprite head[4], tail[3];
    struct SpriteRenderResource graphics;
};

struct SpriteTrailPool {
    struct Task task;
    u32 activeMask;
    u8 unknown1C[4];
    struct SpriteTrail entries[4];
};

struct SpriteTrail *MotionEffectPool_FindFreeEntry(struct SpriteTrailPool *, s32 *);
void SpriteTrail_InitCollider(struct SpriteTrail *);
void SpriteTrail_InitRenderCollider(struct SpriteTrail *);
s32 SpriteTrail_CreateMotionEffect(struct SpriteTrailPool *, struct TrailVector *,
                                   struct TrailVector *, u32, s32, s32);

typedef char SpriteTrailSizeCheck[sizeof(struct SpriteTrail) == 0x258 ? 1 : -1];
typedef char SpriteTrailPoolSizeCheck[sizeof(struct SpriteTrailPool) == 0x980 ? 1 : -1];

#endif
