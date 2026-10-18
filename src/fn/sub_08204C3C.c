#define EnemyD459_FireProjectile   sub_08204C3C
#define Projectile_SpawnFromConfig sub_08202B30
#define gSineTable                 gUnk_086149C4
#include "global.h"

struct EnemyD459Vector {
    s16 x, y, z, pad;
};
struct EnemyD459 {
    u8 pad0[0x40];
    struct EnemyD459Vector position;
    u8 pad48[0x120 - 0x48];
    s16 collisionA, collisionB, collisionC;
    u8 pad126[0x132 - 0x126];
    u8 facing;
    u8 pad133[3];
    s16 movement;
};
extern const s16 gUnk_086149C4[];
void ProjectileConfig_Init(void *);
void ProjectileConfig_SetAppearance(void *, u32, u32, s32);
void ProjectileConfig_SetCollision(void *, u32, u32, u32, u32, u32);
void ProjectileConfig_SetCollisionExtents(void *, u16, u16, u16);
void ProjectileConfig_SetCollisionOffset(void *, u32, u32, u32);
void ProjectileConfig_SetMovement(void *, u32, u32, u32, u32);
void ProjectileConfig_SetAnimation(void *, u32, u32, u32);
s32 sub_08202B30(void *, struct EnemyD459Vector *);
void Sound_PlaySong(u32);
static inline s32 scale(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}
static inline void polar(struct EnemyD459Vector *v, s32 angle, s32 radius)
{
    v->x = scale(gSineTable[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = scale(gSineTable[angle & 255] * radius);
}
static inline void addVector(struct EnemyD459Vector *out, struct EnemyD459Vector *a,
                             struct EnemyD459Vector *b)
{
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}
void sub_08204C3C(struct EnemyD459 *enemy)
{
    u8 config[0x5c];
    struct EnemyD459Vector offset;
    struct EnemyD459Vector position;
    struct EnemyD459Vector *pos = &position;
    position = enemy->position;
    pos->y += 200;
    polar(&offset, (u8)(enemy->facing * 64 + 192), 128);
    addVector(&position, &position, &offset);
    ProjectileConfig_Init(config);
    ProjectileConfig_SetAppearance(config, 0x7e7c, 0, 0x228);
    ProjectileConfig_SetCollision(config, 0, 0x20000, enemy->collisionA, enemy->collisionB,
                                  enemy->collisionC);
    ProjectileConfig_SetCollisionExtents(config, 100, 64, 100);
    ProjectileConfig_SetCollisionOffset(config, 0, -72, 0);
    ProjectileConfig_SetMovement(config, 0, 120, (u8)(enemy->facing * 64 + 192), enemy->movement);
    ProjectileConfig_SetAnimation(config, 0xa6cf, 6, 4);
    sub_08202B30(config, &position);
    Sound_PlaySong(0x516);
}
