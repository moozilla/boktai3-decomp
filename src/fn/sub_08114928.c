#include "scene_callback_pool.h"
#include "ram.h"

struct NativeEffectProfile {
    u8 unknown, type;
    u16 lifetime, unknown4, damage, parameter, threshold;
};
struct SceneCallbackPool *NativeEffect_EnsureManager(void);
u8 EffectPool_CheckCapacity(struct SceneCallbackPool *, s32, s32, s32);
void NativeEffect_InitTypeZeroSprite(struct ScenePoolSlot *);
void NativeEffect_InitDirectionalSprite(struct ScenePoolSlot *, s32);
void NativeEffect_InitThresholdSprite(struct ScenePoolSlot *, s32);
void NativeEffect_InitDirectionalTypeThreeSprite(struct ScenePoolSlot *, s32);
void NativeEffect_InitUnsequencedSprite(struct ScenePoolSlot *, s32);
void NativeEffect_InitDefaultCollider(struct ScenePoolSlot *, void (*)(void), s32);
void NativeEffect_InitColliderWithDepth(struct ScenePoolSlot *, void (*)(void), s32, s32);
void NativeEffect_OnColliderExpire(void);
void NativeEffect_OnColliderImpact(void);
void sub_0811455C(void);
void NativeEffect_UpdateLinearFlight(struct ScenePoolSlot *);
void NativeEffect_UpdateRotatingFlight(struct ScenePoolSlot *);
void NativeEffect_UpdateTurningTrail(struct ScenePoolSlot *);
void NativeEffect_SetState(struct ScenePoolSlot *, void (*)(struct ScenePoolSlot *));
void EffectActor_BeginHorizontalMotion(struct ScenePoolSlot *, struct NativeEffectProfile *, s32);
void Sound_PlaySong(u32);
static inline u32 GroupId(u32 frame, s32 group)
{
    u32 mask = 255;
    return (group & mask) | ((frame & mask) << 8);
}
static inline s32 ThresholdDamage(struct NativeEffectProfile *profile)
{
    if (profile->threshold > 60)
        return profile->damage * 2;
    return profile->damage;
}
/* Select a sprite and motion state from the emission profile. */
s32 NativeEffect_CreateFromProfile(struct NativeEffectVector *position, s32 height,
                                   struct NativeEffectVector *velocity,
                                   struct NativeEffectProfile *profile, s32 group, s32 reverse,
                                   s32 required, s32 allowGroups)
{
    struct SceneCallbackPool *pool;
    s32 damage;
    s32 i;
    if (!gSceneCallbackPool) {
        pool = NativeEffect_EnsureManager();
        if (!pool)
            return -1;
    } else
        pool = gSceneCallbackPool;
    if (EffectPool_CheckCapacity(pool, GroupId(pool->frame, group), required, allowGroups))
        return -1;
    for (i = 0; i < 16; i++) {
        if (!pool->slots[i].active) {
            switch (profile->type) {
                case 0:
                    NativeEffect_InitTypeZeroSprite(&pool->slots[i]);
                    break;

                case 1:
                    NativeEffect_InitDirectionalSprite(&pool->slots[i], reverse);
                    break;

                case 2:
                    NativeEffect_InitThresholdSprite(&pool->slots[i], profile->threshold);
                    break;

                case 3:
                    NativeEffect_InitDirectionalTypeThreeSprite(&pool->slots[i], reverse);
                    break;

                case 4:
                    NativeEffect_InitUnsequencedSprite(&pool->slots[i], reverse);
                    break;
            }

            pool->slots[i].position = *position;
            pool->slots[i].velocity = *velocity;
            pool->slots[i].height = height;
            pool->slots[i].lifetime = profile->lifetime;
            pool->slots[i].sprite.flags |= 0x11;
            pool->slots[i].sprite.priority = 1;
            pool->slots[i].active = 1;
            pool->slots[i].state = 0;
            group = GroupId(pool->frame, group);
            pool->slots[i].id = group;
            switch (profile->type) {
                case 0:
                    pool->slots[i].position.x += 0x4000;
                    pool->slots[i].velocity.x += 0x4000;
                    pool->slots[i].speed = pool->slots[i].velocity.x >> 12;
                    NativeEffect_InitDefaultCollider(&pool->slots[i], NativeEffect_OnColliderExpire,
                                                     profile->damage);
                    NativeEffect_SetState(&pool->slots[i], NativeEffect_UpdateLinearFlight);
                    Sound_PlaySong(0xcd);
                    break;

                case 2:
                    pool->slots[i].position.x += 0xc000;
                    pool->slots[i].velocity.x += 0x5000;
                    pool->slots[i].speed = pool->slots[i].velocity.x >> 12;
                    if (profile->threshold > 60) {
                        damage = ThresholdDamage(profile);
                        NativeEffect_InitDefaultCollider(&pool->slots[i], sub_0811455C, damage);
                    } else {
                        damage = ThresholdDamage(profile);
                        NativeEffect_InitDefaultCollider(&pool->slots[i],
                                                         NativeEffect_OnColliderExpire, damage);
                    }
                    NativeEffect_SetState(&pool->slots[i], NativeEffect_UpdateLinearFlight);
                    Sound_PlaySong(0xcd);
                    break;

                case 1:
                    pool->slots[i].position.x += 0x4000;
                    pool->slots[i].velocity.x += 0x800;
                    pool->slots[i].speed = pool->slots[i].velocity.x;
                    NativeEffect_InitDefaultCollider(&pool->slots[i], NativeEffect_OnColliderExpire,
                                                     profile->damage);
                    NativeEffect_SetState(&pool->slots[i], NativeEffect_UpdateRotatingFlight);
                    Sound_PlaySong(0xcd);
                    break;

                case 3:
                    pool->slots[i].position.x += 0x10000;
                    pool->slots[i].velocity.x += 0x2000;
                    pool->slots[i].speed = pool->slots[i].velocity.x;
                    NativeEffect_InitColliderWithDepth(
                        &pool->slots[i], NativeEffect_OnColliderImpact, profile->damage, 10);
                    NativeEffect_SetState(&pool->slots[i], NativeEffect_UpdateTurningTrail);
                    Sound_PlaySong(0x164);
                    break;

                case 4:
                    EffectActor_BeginHorizontalMotion(&pool->slots[i], profile, reverse);
                    Sound_PlaySong(0x164);
                    break;
            }

            return i;
        }
    }

    return -1;
}
