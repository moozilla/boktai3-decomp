#include "global.h"
#include "bosses/ratatosk/ratatosk.h"

extern s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
void Projectile_SetBehaviorMode(u8 *, u16);
void Projectile_SetLaunchParameters(u8 *, u16, u32, u8, u32);
void Projectile_SetScriptProcedure(u8 *, u32, u16);
void Projectile_SetSpriteParameters(u8 *, u32, u32, u32);
void Projectile_SetTrackingTarget(u8 *, u32);
void Sound_PlaySong(u32);
void Projectile_InitParameters(u8 *);
int Projectile_SpawnAtPosition(u8 *, struct ContactPosition *);
static inline int trunc12(int n)
{
    return n >= 0 ? n >> 12 : -(-n >> 12);
}
static inline void sum(struct ContactPosition *dst, struct ContactPosition *a,
                       struct ContactPosition *b)
{
    dst->x = b->x + a->x;
    dst->y = b->y + a->y;
    dst->z = b->z + a->z;
}
static inline void polar(struct ContactPosition *v, s32 angle, s32 radius)
{
    s32 x = trunc12(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->x = x;
    v->y = 0;
    v->z = trunc12(gUnk_086149C4[angle & 255] * radius);
}

void SceneActors_FireTrackingProjectileSpread(struct Ratatosk *p, struct ContactPosition *position,
                                              u32 parameter)
{
    u8 projectile[0x58];
    struct ContactPosition delta;
    struct ContactPosition spawn;
    u8 angle;
    s32 i;
    struct ContactPosition *target;
    struct ContactPosition *spawnPtr;
    u16 step;

    target = &p->shotTarget;
    angle = Math_Atan2(target->x - position->x, target->z - position->z);
    step = 16;
    polar(&delta, (u8)((p->facing << 6) - 64), 180);
    sum(&spawn, &delta, position);
    spawn.y += 200;
    angle -= step;
    spawnPtr = &spawn;
    for (i = 0; i < 3; i++) {
        Projectile_InitParameters(projectile);
        Projectile_SetLaunchParameters(projectile, 0, parameter, angle, 24);
        Projectile_SetSpriteParameters(projectile, 0x8e5a, 0, 498);
        Projectile_SetScriptProcedure(projectile, 0x4363, 0);
        Projectile_SetTrackingTarget(projectile, (u32)&p->shotTarget);
        Projectile_SetBehaviorMode(projectile, 1);
        Projectile_SpawnAtPosition(projectile, spawnPtr);
        angle += step;
    }
    Sound_PlaySong(1384);
}
