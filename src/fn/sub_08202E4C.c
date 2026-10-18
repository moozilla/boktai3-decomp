#include "global.h"
void Memory_Clear(void *, u32);
void ProjectileConfig_SetMovement(void *, u32, u32, u32, u32);
void ProjectileConfig_SetCollision(void *, u32, u32, u32, u32, u32);
void ProjectileConfig_SetCollisionExtents(void *, u32, u32, u32);
void ProjectileConfig_SetCollisionOffset(void *, u32, u32, u32);
void ProjectileConfig_SetAppearance(void *, u32, u32, s32);
void ProjectileConfig_SetAnimation(void *, u32, u32, u32);
void ProjectileConfig_SetUpdateCallback(void *, void *);
void ProjectileConfig_SetVerticalMotion(void *, u32, u32);
u32 TimedSpriteEffect_UpdateDefault(void);
void ProjectileConfig_Init(u8 *p)
{
    u32 z;
    Memory_Clear(p, 0x5c);
    z = 0;
    ProjectileConfig_SetMovement(p, 0, 0x3c, 0, z);
    ProjectileConfig_SetCollision(p, 0, 0, 0xa, 0x14, 0x14);
    ProjectileConfig_SetCollisionExtents(p, 8, 0x20, 8);
    ProjectileConfig_SetCollisionOffset(p, 0, 0, 0);
    ProjectileConfig_SetAppearance(p, 0x848F, 0, -1);
    ProjectileConfig_SetAnimation(p, 0, 0, 0);
    ProjectileConfig_SetUpdateCallback(p, TimedSpriteEffect_UpdateDefault);
    ProjectileConfig_SetVerticalMotion(p, 0, 0);
    *(u16 *)(p + 0x4e) = z;
    *(u16 *)(p + 0x58) = z;
}
