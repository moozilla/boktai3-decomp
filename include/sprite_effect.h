#ifndef GUARD_SPRITE_EFFECT_H
#define GUARD_SPRITE_EFFECT_H
#include "engine.h"
#include "scene_vec.h"
#include "contact_collider.h"
#include "hit_spark_random.h"
/* Word copies and signed halfword motion share the same eight-byte vector. */
union SpriteEffectVector {
    struct SceneVec coordinates;
    struct {
        u32 xy, z;
    } words;
    struct {
        s32 x : 16, y : 16, z : 16, pad : 16;
    } packed;
};
struct SpriteEffectSource {
    u8 unknown0[9];
    u8 facing;
    u8 unknownA[2];
    union SpriteEffectVector position;
};
struct SpriteEffect {
    u32 flags;
    u8 unknown4[3], layer;
    u8 unknown8[4];
    void *animation;
    u16 frame;
    u8 unknown12[0xA];
    union SpriteEffectVector previous;
    u8 unknown24[8];
    union SpriteEffectVector position, motion;
    struct ContactCollider collider;
    u8 shadow[0x40];
    s32 timer;
    u16 state;
    u8 active;
    s8 slot;
};
struct SpriteEffectPool {
    struct Task task;
    u8 spriteData[0x1C];
    u32 used;
    struct SpriteEffect effects[8];
};
/* Adjacent animated-body controller; shares no storage with the shard pool. */
struct SceneActor14 {
    u8 unknown0[8];
    u32 flags;
    u8 unknownC[0x11], animationFlags;
    u8 unknown1E[0x42];
    u8 body[0xD0];
    s16 state, base, step, frame, counter, phase, frameLimit, slot;
    u16 bodyFlags;
};
struct SceneActor14Manager {
    struct Task task;
    u8 animation[0x534];
    u32 used;
};
extern const u8 gUnk_086106E0[], gUnk_086106F0[];
extern void *gUnk_086106E4[];
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void Shadow_Destroy(void *);
void Shadow_Initialize(void *, void *, u32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
void sub_08014634(u32, u32, void *, void *, void *, u32, u32);
s32 Mod(s32, s32);
s32 Div(s32, s32);
void TerrainOverlay_QueueRemoveActor(void *);
void SceneBody_Unregister(void *);
void SceneActor14_OnColliderContact(void);
struct SpriteEffectPool *SceneActor14_EnsureSingleton(void);
struct SpriteEffect *SpriteEffectPool_AllocateSlot(struct SpriteEffectPool *);
s32 SpriteEffectPool_Update(struct SpriteEffectPool *);
s32 SpriteEffectPool_RemoveSlots(struct SpriteEffectPool *);
s32 SceneActor14_InitManager(struct SpriteEffectPool *);
s32 SpriteEffect_SpawnWithMotion(struct SpriteEffectSource *, struct SpriteEffectSource *,
                                 union SpriteEffectVector *, u32, u32, u32, u32);
#endif
