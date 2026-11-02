#include "pink_spine_playback.h"
#include "projectile_pool.h"
extern const s16 gUnk_086149C4[];
void ProjectileConfig_Init(void *);
void ProjectileConfig_SetAppearance(void *, u32, u32, s32);
void ProjectileConfig_SetCollision(void *, u32, u32, u32, u32, u32);
void ProjectileConfig_SetMovement(void *, u32, u32, u32, u32);
void ProjectileConfig_SetAnimation(void *, u32, u32, u32);
static inline s32 scale(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}
static inline void polar(struct ContactPosition *v, int angle, int radius)
{
    v->x = scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = scale(gUnk_086149C4[angle & 255] * radius);
}
static inline void add(struct ContactPosition *d, struct ContactPosition *p,
                       struct ContactPosition *v)
{
    d->x = p->x + v->x;
    d->y = p->y + v->y;
    d->z = p->z + v->z;
}
void Combat_SpawnDirectionalProjectile(struct PinkSpineState *emitter, u8 direction)
{
    struct ProjectileConfig config;
    struct ContactPosition position;
    struct ContactPosition offset;
    PinkSpine_ProjectileOrigin(&position, emitter);
    polar(&offset, (u8)(((direction + 5) & 7) << 5), 0x40);
    add(&position, &position, &offset);
    ProjectileConfig_Init(&config);
    ProjectileConfig_SetAppearance(&config, 0x7E7C, 0, 0x1BD);
    ProjectileConfig_SetCollision(&config, 0, 0, emitter->projectilePower,
                                  emitter->projectileKnockback, emitter->projectileImmunity);
    ProjectileConfig_SetMovement(&config, 0, 0x78, (u8)(((direction + 5) & 7) << 5),
                                 emitter->projectileSpeed);
    ProjectileConfig_SetAnimation(&config, 0xA6CF, 3, 8);
    Projectile_SpawnFromConfig(&config, &position);
}
