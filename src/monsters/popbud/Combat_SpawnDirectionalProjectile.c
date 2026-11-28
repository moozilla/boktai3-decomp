#include "monsters/popbud/popbud_playback.h"
#include "effects/projectiles/projectile_pool.h"
#include "engine/math/math_shift.h"
extern const s16 gUnk_086149C4[];
void ProjectileConfig_Init(void *);
void ProjectileConfig_SetAppearance(void *, u32, u32, s32);
void ProjectileConfig_SetCollision(void *, u32, u32, u32, u32, u32);
void ProjectileConfig_SetMovement(void *, u32, u32, u32, u32);
void ProjectileConfig_SetAnimation(void *, u32, u32, u32);
static inline void polar(struct ContactPosition *v, int angle, int radius)
{
    v->x = Math_ShiftTowardZero(gUnk_086149C4[(angle + 64) & 255] * radius, 12);
    v->y = 0;
    v->z = Math_ShiftTowardZero(gUnk_086149C4[angle & 255] * radius, 12);
}
static inline void add(struct ContactPosition *d, struct ContactPosition *p,
                       struct ContactPosition *v)
{
    d->x = p->x + v->x;
    d->y = p->y + v->y;
    d->z = p->z + v->z;
}
void Combat_SpawnDirectionalProjectile(struct PinkSpineState *popbud, u8 direction)
{
    struct ProjectileConfig config;
    struct ContactPosition position;
    struct ContactPosition offset;
    Popbud_ProjectileOrigin(&position, popbud);
    polar(&offset, (u8)(((direction + 5) & 7) << 5), 0x40);
    add(&position, &position, &offset);
    ProjectileConfig_Init(&config);
    ProjectileConfig_SetAppearance(&config, 0x7E7C, 0, 0x1BD);
    ProjectileConfig_SetCollision(&config, 0, 0, popbud->projectilePower,
                                  popbud->projectileKnockback, popbud->projectileImmunity);
    ProjectileConfig_SetMovement(&config, 0, 0x78, (u8)(((direction + 5) & 7) << 5),
                                 popbud->projectileSpeed);
    ProjectileConfig_SetAnimation(&config, 0xA6CF, 3, 8);
    Projectile_SpawnFromConfig(&config, &position);
}
