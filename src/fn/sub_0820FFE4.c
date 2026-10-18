#define gCombatSineTable gUnk_086149C4
#include "global.h"
/* Names and field interpretations are tentative. */
struct ProjectilePosition {
    s16 x, y, z, pad;
};
struct ProjectileEmitter {
    u8 pad0[0x40];
    struct ProjectilePosition position;
    u8 pad48[0xDC];
    s16 collisionX, collisionY, collisionZ;
    u8 pad12A[4];
    s16 movement;
};
extern const s16 gUnk_086149C4[];
void ProjectileConfig_Init(void *);
void ProjectileConfig_SetAppearance(void *, u32, u32, s32);
void ProjectileConfig_SetCollision(void *, u32, u32, u32, u32, u32);
void ProjectileConfig_SetMovement(void *, u32, u32, u32, u32);
void ProjectileConfig_SetAnimation(void *, u32, u32, u32);
void Projectile_SpawnFromConfig(void *, void *);
static inline s32 scale(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}
static inline void polar(struct ProjectilePosition *v, int angle, int radius)
{
    v->x = scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = scale(gUnk_086149C4[angle & 255] * radius);
}
static inline void add(struct ProjectilePosition *d, struct ProjectilePosition *p,
                       struct ProjectilePosition *v)
{
    d->x = p->x + v->x;
    d->y = p->y + v->y;
    d->z = p->z + v->z;
}
static inline void copy(struct ProjectilePosition *d, struct ProjectilePosition *s)
{
    *d = *s;
}
static inline void raisePosition(struct ProjectilePosition *p)
{
    p->y += 0x80;
}
void Combat_SpawnDirectionalProjectile(struct ProjectileEmitter *emitter, u8 direction)
{
    u8 config[0x5C];
    struct ProjectilePosition position;
    struct ProjectilePosition offset;
    u8 angle;
    copy(&position, &emitter->position);
    raisePosition(&position);
    polar(&offset, (u8)(((direction + 5) & 7) << 5), 0x40);
    add(&position, &position, &offset);
    ProjectileConfig_Init(config);
    ProjectileConfig_SetAppearance(config, 0x7E7C, 0, 0x1BD);
    ProjectileConfig_SetCollision(config, 0, 0, emitter->collisionX, emitter->collisionY,
                                  emitter->collisionZ);
    ProjectileConfig_SetMovement(config, 0, 0x78, (u8)(((direction + 5) & 7) << 5),
                                 emitter->movement);
    ProjectileConfig_SetAnimation(config, 0xA6CF, 3, 8);
    Projectile_SpawnFromConfig(config, &position);
}
