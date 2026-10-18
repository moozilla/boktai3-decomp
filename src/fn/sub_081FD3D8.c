
#include "global.h"

struct ProjectileParameters {
    u8 unk00[0x38];
    u32 unk38;
    u8 unk3C[0x0C];
    u32 unk48;
    u16 unk4C;
    u8 unk4E[4];
    u16 unk52;
    u16 unk54;
    u8 unk56[2];
};

void Memory_Clear(void *, u32);
void Projectile_SetLaunchParameters(struct ProjectileParameters *, u16, u16, u8, u32);
void Projectile_SetCollisionParameters(struct ProjectileParameters *, u32, u32, u32, u32, u32);
void Projectile_SetCollisionExtents(struct ProjectileParameters *, u16, u16, u16);
void Projectile_SetCollisionOffset_081FD49C(struct ProjectileParameters *, u16, u16, u16);
void Projectile_SetSpriteParameters(struct ProjectileParameters *, u32, u32, u32);
void Projectile_SetBehaviorCallback(struct ProjectileParameters *, u32 (*)(void *));
u32 SceneProjectile_UpdatePulse(void *);

/* Default configuration for the tentative scene projectile family. */
void Projectile_InitParameters(struct ProjectileParameters *p)
{
    Memory_Clear(p, 0x58);
    Projectile_SetLaunchParameters(p, 0, 0x3C, 0, 0);
    Projectile_SetCollisionParameters(p, 0, 0, 0xA, 0x14, 0x14);
    Projectile_SetCollisionExtents(p, 0x10, 0x80, 0x10);
    Projectile_SetCollisionOffset_081FD49C(p, 0, 0, 0);
    Projectile_SetSpriteParameters(p, 0x848F, 0, -1);
    Projectile_SetBehaviorCallback(p, SceneProjectile_UpdatePulse);
    p->unk48 = 0;
    p->unk4C = 0;
    p->unk38 = 0;
    p->unk52 = 0;
    p->unk54 = 0;
}
