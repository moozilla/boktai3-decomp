#include "global.h"

// The Task4E01 projectile's defaults, in the shape of Projectile_InitParameters.
struct ProjectileParams {
    u16 unk00;
    u8 unk02[0x36];
    u16 unk38;
    u16 unk3A;
    u8 unk3C[0x1C];
};
void Memory_Clear(void *, u32);
void Projectile_SetFlightParams(void *, s32, s32, s32, s32);
void Projectile_SetDamageParams(void *, s32, s32, s32, s32, s32);
void Projectile_SetCollisionSize(void *, s32, s32, s32);
void Projectile_SetCollisionOffset(void *, s32, s32, s32);
void Projectile_SetCollisionMask(void *, s32);
void Projectile_SetSpriteParams(void *, u32, u32, s32);
void Projectile_SetUpdateCallback(void *, void *);
void Task4E01_OnDefaultTimedAnimation(void);
void Projectile_InitializeParams(struct ProjectileParams *p)
{
    Memory_Clear(p, 0x58);
    Projectile_SetFlightParams(p, 0, 0x3C, 0, 0);
    Projectile_SetDamageParams(p, 0, 0, 0xA, 0x14, 0x14);
    Projectile_SetCollisionSize(p, 8, 0x20, 8);
    Projectile_SetCollisionOffset(p, 0, -0x6C, 0);
    Projectile_SetCollisionMask(p, 1);
    Projectile_SetSpriteParams(p, 0x848F, 0, -1);
    Projectile_SetUpdateCallback(p, Task4E01_OnDefaultTimedAnimation);
    p->unk00 = 0;
    p->unk38 = 0;
    p->unk3A = 0;
}
