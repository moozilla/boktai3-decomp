#ifndef GUARD_NATIVE_EFFECT_H
#define GUARD_NATIVE_EFFECT_H
#include "effects/particles/scene_callback_pool.h"
#include "link/wireless/scene_pool.h"
#include "effects/particles/scene_debris.h"
struct NativeEffectProfile {
    u8 unknown, type;
    u16 lifetime, unknown4, damage, parameter, threshold;
};
struct NativeEffectPacked {
    s16 x, y, z, pad;
};
struct NativeEffectPackedBits {
    u32 x : 16, y : 16, z : 16, pad : 16;
};
struct Camera {
    u8 unknown0[0x44];
    s32 x;
};
void ProjectileEffect_SetState(struct ScenePoolSlot *, void (*)(struct ScenePoolSlot *, u32));
void ProjectileEffect_DestroyEntry(struct ScenePoolSlot *);
void SpriteEffect_RemoveWithBurst(struct ScenePoolSlot *);
s32 SceneGroundEffect_IsBelowGround(struct ScenePoolSlot *, s32);
void ProjectileEffect_OnColliderExpire(struct ScenePoolSlot *);
void ProjectileEffect_OnColliderImpact(struct ScenePoolSlot *);
void ProjectileEffect_OnColliderIgnore(void);
void ProjectileEffect_InitDefaultCollider(struct ScenePoolSlot *, void (*)(void), u32);
void ProjectileEffect_InitColliderWithDepth(struct ScenePoolSlot *, void (*)(void), u32, u32);
s32 SceneCallbackPool_Initialize(struct SceneCallbackPool *);
s32 SceneCallbackPool_Update(struct SceneCallbackPool *);
s32 ProjectileEffect_DestroyManager(void);
struct SceneCallbackPool *ProjectileEffect_EnsureManager(void);
s32 SlotRegistry_ClaimMatchingSlot(struct SceneCallbackPool *, s32, s32, u8 *);
s32 EffectPool_CheckCapacity(struct SceneCallbackPool *, s32, s32, s32);
void SlotRegistry_RemoveSprite(u32);
#endif
