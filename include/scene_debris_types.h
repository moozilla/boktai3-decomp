#ifndef GUARD_SCENE_DEBRIS_TYPES_H
#define GUARD_SCENE_DEBRIS_TYPES_H
#include "display_animation.h"
union SceneDebrisVisual {
    struct DisplayAnimationSequence sequence;
    struct DisplayAnimationKeyframes keyframes;
};
struct SceneDebris {
    union SceneDebrisVisual *visual;
    struct ContactPosition position, velocity;
    u32 timer;
    u8 restitution, active, entered, visualType;
    void (*update)(struct SceneDebris *);
};
struct DebrisPool {
    struct Task task;
    struct SceneDebris entries[64];
};
void SceneDebris_ReleaseVisual(struct SceneDebris *);
void SceneDebris_OnMotionEnd(struct SceneDebris *);
void SceneDebris_UpdateAnimatedMotion(struct SceneDebris *);
void SceneDebris_UpdateBouncingMotion(struct SceneDebris *);
s32 SceneDebrisPool_Update(struct DebrisPool *);
s32 SceneDebrisPool_Destroy(struct DebrisPool *);
s32 SceneDebrisPool_Initialize(struct DebrisPool *);
struct DebrisPool *SceneDebrisPool_EnsureSingleton(void);
struct Camera {
    u8 unknown0[0x44];
    s32 x, y;
};
#endif
