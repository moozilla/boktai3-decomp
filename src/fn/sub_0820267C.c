#include "projectile_pool.h"
void ProjectileConfig_Init(void *);
void Sprite_InitUnlinked(void *, void *, u32);
void Collider_SetCallback(void *, void *, void *);
void Collider_SetAttackProperties(void *, s32, s32, u32, u32, s32);
void TimedSpriteEffect_OnColliderContact(void);
static inline void Fill32(void *dst, u32 size)
{
    u32 tmp = 0;
    CpuSet(&tmp, dst, 0x05000000 | size / 4);
}
void TimedSpriteEffect_InitSlot(struct TimedProjectilePool *pool, struct ProjectileSlot *p,
                                s32 index)
{
    struct ProjectileConfig *data = &p->params;
    void *obj, *effect;
    ProjectileConfig_Init(data);
    p->timer = 0;
    Fill32(&p->velocity, sizeof(p->velocity));
    obj = p->graphics;
    AnimData_Load(obj, data->graphicsId);
    Sprite_InitUnlinked(&p->sprite, obj, 1);
    effect = &p->collider;
    Collider_Init(effect, 0, 0x2001, 0, 16, &data->bounds, &data->offset);
    Collider_SetCallback(effect, TimedSpriteEffect_OnColliderContact, p);
    Collider_SetAttackProperties(effect, data->power, data->knockback, data->rewardFlags,
                                 data->attackFlags, data->immunity);
}
