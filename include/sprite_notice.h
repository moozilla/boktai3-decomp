#ifndef GUARD_SPRITE_NOTICE_H
#define GUARD_SPRITE_NOTICE_H
#include "engine.h"
#include "scene_particle_pool.h"
struct SpriteNotice {
    struct SceneParticleEffect sprite;
    struct DisplaySequenceState animation;
    u8 unknown38[10];
    u8 state;
    s8 index;
    u8 entered, unknown45[3];
};
struct SpriteNoticeManager {
    struct Task task;
    void *animationData;
    const u16 *spriteData;
    struct SpriteNotice slots[32];
    u32 occupied;
};
struct SpriteNotice *SpriteNotice_AllocateSlot(struct SpriteNoticeManager *);
struct SpriteNoticeManager *SceneTrailEffect_EnsurePool(void);
s32 SceneTrailEffect_InitPool(struct SpriteNoticeManager *);
s32 SpriteNotice_TickManager(struct SpriteNoticeManager *);
u32 SceneAnimatedEffect_OnTaskDestroy(struct SpriteNoticeManager *);
void SceneAnimatedEffect_LoadResources(struct SpriteNoticeManager *);
s32 SpriteNotice_Create(const struct ContactPosition *, u32);
#endif
