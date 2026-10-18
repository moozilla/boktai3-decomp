#include "global.h"
#define VulcanCannon_FireProjectile sub_082069A8
#define Projectile_CreateFromConfig sub_08202B30
#define gSineTable                  gUnk_086149C4
struct CannonVector {
    s16 x, y, z, pad;
};
struct VulcanCannon {
    u8 pad0[0x88];
    struct CannonVector position;
    u8 pad90[0x90];
    s16 attack, element, power;
    u8 pad126[0xc];
    s8 facing;
    u8 pad133[3];
    s16 speed;
};
void ProjectileConfig_Init(void *);
void ProjectileConfig_SetAppearance(void *, u32, u32, s32);
void ProjectileConfig_SetCollision(void *, u32, u32, u32, u32, u32);
void ProjectileConfig_SetMovement(void *, u32, u32, u32, u32);
void ProjectileConfig_SetAnimation(void *, u32, u32, u32);
void sub_08202B30(void *, void *);
extern const s16 gUnk_086149C4[];
static inline s32 scale(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}
static inline void polar(struct CannonVector *v, s32 angle, s32 radius)
{
    v->x = scale(gSineTable[angle + 64] * radius);
    v->y = 0;
    v->z = scale(gSineTable[angle] * radius);
}
static inline void add(struct CannonVector *a, struct CannonVector *b)
{
    a->x += b->x;
    a->y += b->y;
    a->z += b->z;
}
static inline void copy(struct CannonVector *a, struct CannonVector *b)
{
    *a = *b;
}
static inline void raise(struct CannonVector *v)
{
    v->y += 128;
}
void sub_082069A8(struct VulcanCannon *p)
{
    u8 config[0x5c];
    struct CannonVector position, offset;
    s32 facing = p->facing;
    s32 angle = 64;
    if (facing == 0)
        angle = 0;
    copy(&position, &p->position);
    raise(&position);
    polar(&offset, angle, 160);
    add(&position, &offset);
    ProjectileConfig_Init(config);
    ProjectileConfig_SetAppearance(config, 0x7e7c, 0, 0x1c2);
    ProjectileConfig_SetCollision(config, 0, 0, p->attack, p->element, p->power);
    ProjectileConfig_SetMovement(config, 0, 120, angle, p->speed);
    ProjectileConfig_SetAnimation(config, 0xa6cf, 7, 0);
    Projectile_CreateFromConfig(config, &position);
}
