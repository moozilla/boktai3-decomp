#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Vec {
    s16 x, y, z, pad;
};
struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct Actor {
    u8 pad0[0xa8];
    u8 facing;
    u8 padA9[0xf10 - 0xa9];
    u8 target[1];
};
void Projectile_InitParameters(void *);
void Projectile_SetLaunchParameters(void *, u16, u32, u8, u32);
void Projectile_SetHomingParameters(void *, s32, void *, s32);
void Projectile_SetSpriteParameters(void *, u32, u32, u32);
void Projectile_SetScriptProcedure(void *, u32, u16);
void Projectile_SetTrackingTarget(void *, void *);
void Projectile_SetBehaviorMode(void *, u16);
void Projectile_SpawnAtPosition(void *, struct Vec *);
void Sound_PlaySong(u32);
static inline void sum(struct Vec *d, struct Vec *a, struct Vec *b)
{
    d->x = b->x + a->x;
    d->y = a->y + b->y;
    d->z = b->z + a->z;
}
// confidence=low: fires a tracking projectile at a facing-dependent offset.
void Ratatosk_FireTrackingShot(struct Actor *p, struct Vec *position, u32 parameter)
{
    u8 params[0x58];
    struct PackedVec offset;
    struct Vec spawn;
    register struct Actor *p2 asm("r8") = p;
    s32 direction = 0;
    switch (p2->facing) {
        case 0:
            break;
        case 1:
            direction = 2;
            break;
        case 2:
            direction = 6;
            break;
    }
    Projectile_InitParameters(params);
    Projectile_SetLaunchParameters(params, 0, parameter, (direction + 5) << 5, 24);
    Projectile_SetHomingParameters(params, 300, p->target, 1);
    Projectile_SetSpriteParameters(params, 0x8e5a, 0, 499);
    Projectile_SetScriptProcedure(params, 0x4363, 3);
    Projectile_SetTrackingTarget(params, p->target);
    Projectile_SetBehaviorMode(params, 0);
    switch (p2->facing) {
        case 0:
            offset.x = 96;
            offset.y = 0;
            offset.z = -184;
            break;
        case 1:
            offset.x = 112;
            offset.y = 0;
            offset.z = -28;
            break;
        case 2:
            offset.x = -28;
            offset.y = 0;
            offset.z = 112;
            break;
        case 3:
            offset.x = -184;
            offset.y = 0;
            offset.z = 96;
            break;
    }
    sum(&spawn, (struct Vec *)&offset, position);
    spawn.y += 150;
    Projectile_SpawnAtPosition(params, &spawn);
    Sound_PlaySong(0x56a);
}
