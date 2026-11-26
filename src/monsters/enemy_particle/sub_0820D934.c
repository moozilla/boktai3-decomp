#include "global.h"
void Memory_Clear(void *, u32);
void EnemyParticle_SetMotionParameters(void *, u32, u32, u32, u32);
void EnemyParticle_SetGraphicsParameters(void *, u32, u32, s32);
void EnemyParticle_SetAnimationParameters(void *, u32, u32, u32);
void EnemyParticle_SetUpdateCallback(void *, void *);
void EnemyParticle_SetVerticalMotion(void *, u32, u32);
void EnemyParticle_UpdateLifetime(void);
void EnemyParticle_InitParameters(u8 *p)
{
    Memory_Clear(p, 0x38);
    EnemyParticle_SetMotionParameters(p, 0, 0x3c, 0, 0);
    EnemyParticle_SetGraphicsParameters(p, 0x848f, 0, -1);
    EnemyParticle_SetAnimationParameters(p, 0, 0, 0);
    EnemyParticle_SetUpdateCallback(p, EnemyParticle_UpdateLifetime);
    EnemyParticle_SetVerticalMotion(p, 0, 0);
    *(u32 *)(p + 0xc) = 0;
}
