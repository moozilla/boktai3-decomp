#define gSineTable gUnk_086149C4
#include "monsters/axe_armor/axe_armor.h"
#include "effects/projectiles/projectile_pool.h"

extern const s16 gUnk_086149C4[];
void ProjectileConfig_Init(void *);
void ProjectileConfig_SetAppearance(void *, u32, u32, s32);
void ProjectileConfig_SetCollision(void *, u32, u32, u32, u32, u32);
void ProjectileConfig_SetCollisionExtents(void *, u16, u16, u16);
void ProjectileConfig_SetCollisionOffset(void *, u32, u32, u32);
void ProjectileConfig_SetMovement(void *, u32, u32, u32, u32);
void ProjectileConfig_SetAnimation(void *, u32, u32, u32);
void Sound_PlaySong(u32);
static inline s32 scale(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}
static inline void polar(struct ContactPosition *v, s32 angle, s32 radius)
{
    v->x = scale(gSineTable[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = scale(gSineTable[angle & 255] * radius);
}
static inline void addVector(struct ContactPosition *out, struct ContactPosition *a,
                             struct ContactPosition *b)
{
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}
void AxeArmor_FireProjectile(struct AxeArmor *enemy)
{
    struct ProjectileConfig config;
    struct ContactPosition offset;
    struct ContactPosition position;
    struct ContactPosition *pos = &position;
    position = enemy->sprite.position;
    pos->y += 200;
    polar(&offset, (u8)((u8)enemy->facing * 64 + 192), 128);
    addVector(&position, &position, &offset);
    ProjectileConfig_Init(&config);
    ProjectileConfig_SetAppearance(&config, 0x7e7c, 0, 0x228);
    ProjectileConfig_SetCollision(&config, 0, 0x20000, enemy->attackA, enemy->attackB,
                                  enemy->attackC);
    ProjectileConfig_SetCollisionExtents(&config, 100, 64, 100);
    ProjectileConfig_SetCollisionOffset(&config, 0, -72, 0);
    ProjectileConfig_SetMovement(&config, 0, 120, (u8)((u8)enemy->facing * 64 + 192), enemy->attackD);
    ProjectileConfig_SetAnimation(&config, 0xa6cf, 6, 4);
    Projectile_SpawnFromConfig(&config, &position);
    Sound_PlaySong(0x516);
}
