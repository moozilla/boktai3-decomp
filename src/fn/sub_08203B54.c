#define EnemyF426_SpawnProjectile sub_08203B54
#define Projectile_SpawnFromConfig sub_08202B30
#define gSineTable gUnk_086149C4
#include "global.h"

// Tentative family name. Spawns at position + (0, 300, 0), offset 128
// along direction * 64 + 192; sets projectile appearance 0x7e7c and sound 0x515.
// Collision arguments at 0x120-0x124 and movement argument at 0x136
// retain neutral names until their setter contracts are better understood.

struct EnemyF426Position {
    s16 x, y, z, unused;
};

struct EnemyF426 {
    u8 unk0[0x40];
    struct EnemyF426Position position;
    u8 unk48[0x120 - 0x48];
    s16 collisionArg0;
    s16 collisionArg1;
    s16 collisionArg2;
    u8 unk126[0x132 - 0x126];
    u8 direction;
    u8 unk133[3];
    s16 movementArg;
};

extern const s16 gUnk_086149C4[];
void ProjectileConfig_Init(void *);
void ProjectileConfig_SetAppearance(void *, u32, u32, s32);
void ProjectileConfig_SetCollision(void *, u32, u32, s32, s32, s32);
void ProjectileConfig_SetCollisionExtents(void *, u32, u32, u32);
void ProjectileConfig_SetCollisionOffset(void *, s32, s32, s32);
void ProjectileConfig_SetMovement(void *, u32, u32, u32, s32);
void ProjectileConfig_SetAnimation(void *, u32, u32, u32);
s32 sub_08202B30(void *, struct EnemyF426Position *);
void Sound_PlaySong(u32);

static inline s32 truncate12(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}

static inline void polar(struct EnemyF426Position *v, u8 angle, s32 radius)
{
    v->x = truncate12(gSineTable[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = truncate12(gSineTable[angle] * radius);
}

void sub_08203B54(struct EnemyF426 *actor)
{
    u8 config[0x5c];
    struct EnemyF426Position delta;
    struct EnemyF426Position position;
    {
        struct EnemyF426Position *pos = &position;
        *pos = actor->position;
        pos->y += 300;
    }
    polar(&delta, (actor->direction << 6) + 192, 128);
    {
        struct EnemyF426Position *p = &position;
        struct EnemyF426Position *d = &delta;
        const struct EnemyF426Position *first = &delta;
        position.x += first->x;
        p->y += d->y;
        p->z += d->z;
    }
    ProjectileConfig_Init(config);
    ProjectileConfig_SetAppearance(config, 0x7e7c, 0, 0x229);
    ProjectileConfig_SetCollision(config, 0, 0x200000, actor->collisionArg0,
                                  actor->collisionArg1, actor->collisionArg2);
    ProjectileConfig_SetCollisionExtents(config, 64, 96, 64);
    ProjectileConfig_SetCollisionOffset(config, 0, -172, 0);
    ProjectileConfig_SetMovement(config, 0, 120, (u8)((actor->direction << 6) + 192),
                                actor->movementArg);
    ProjectileConfig_SetAnimation(config, 0xa6cf, 5, 4);
    Projectile_SpawnFromConfig(config, &position);
    Sound_PlaySong(0x515);
}
