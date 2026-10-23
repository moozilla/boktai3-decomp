#include "global.h"

#ifdef NONMATCHING
// fuzzy: 86.7%
// foundry draft (worker best 84.1%); what differs is mostly register allocation and instruction order

#define Projectile_Spawn Projectile_SpawnFromConfig
#define gSineTable       gUnk_086149C4
struct OrangeSpinePosition {
    s16 x, y, z, pad;
} __attribute__((aligned(4)));
struct OrangeSpine {
    u8 unk0[0x40];
    struct OrangeSpinePosition position;
    u8 unk48[0x120 - 0x48];
    s16 collisionX, collisionY, collisionZ;
    u8 unk126[0x132 - 0x126];
    s8 facing;
    u8 unk133[3];
    s16 movement;
    u8 unk138[2];
    s16 speed;
};
extern const s16 gUnk_086149C4[];
void ProjectileConfig_Init(void *);
void ProjectileConfig_SetAppearance(void *, u32, u32, s32);
void ProjectileConfig_SetCollision(void *, u32, u32, u32, u32, u32);
void ProjectileConfig_SetMovement(void *, u32, u32, u32, u32);
void ProjectileConfig_SetAnimation(void *, u32, u32, u32);
void Projectile_SpawnFromConfig(void *, struct OrangeSpinePosition *);
void Sound_PlaySong(u32);
static inline s32 scale(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
static inline void vector(struct OrangeSpinePosition *p, s32 angle, s32 magnitude)
{
    p->x = scale(gSineTable[(angle + 64) & 255] * magnitude);
    p->y = 0;
    p->z = scale(gSineTable[angle & 255] * magnitude);
}
static inline void add(struct OrangeSpinePosition *out, struct OrangeSpinePosition *a,
                       struct OrangeSpinePosition *b)
{
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = b->z + a->z;
}
void OrangeSpine_FireProjectile(struct OrangeSpine *p)
{
    u8 config[0x5c];
    struct OrangeSpinePosition delta;
    struct OrangeSpinePosition position;
    struct OrangeSpinePosition *pos;
    s32 facing = p->facing;
    s32 angle = 64;
    angle &= -(facing != 0);
    pos = &position;
    *pos = p->position;
    pos->y += 128;
    vector(&delta, angle, 160);
    add(&position, pos, &delta);
    ProjectileConfig_Init(config);
    ProjectileConfig_SetAppearance(config, 0x7e7c, 0, 0x1c1);
    ProjectileConfig_SetCollision(config, 0, 0x80000, p->collisionX, p->collisionY, p->collisionZ);
    ProjectileConfig_SetMovement(config, 0, p->speed, angle, p->movement);
    ProjectileConfig_SetAnimation(config, 0xa6cf, 4, 0);
    Projectile_Spawn(config, pos);
    Sound_PlaySong(0x51e);
}
#else
INCLUDE_ASM("asm/nonmatching", OrangeSpine_FireProjectile);
#endif
