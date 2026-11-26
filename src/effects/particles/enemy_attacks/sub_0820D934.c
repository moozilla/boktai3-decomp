#include "global.h"
void Memory_Clear(void *, u32);
void EnemyAttackParticle_SetMotionParameters(void *, u32, u32, u32, u32);
void EnemyAttackParticle_SetGraphicsParameters(void *, u32, u32, s32);
void EnemyAttackParticle_SetAnimationParameters(void *, u32, u32, u32);
void EnemyAttackParticle_SetUpdateCallback(void *, void *);
void EnemyAttackParticle_SetVerticalMotion(void *, u32, u32);
void EnemyAttackParticle_UpdateLifetime(void);
void EnemyAttackParticle_InitParameters(u8 *p)
{
    Memory_Clear(p, 0x38);
    EnemyAttackParticle_SetMotionParameters(p, 0, 0x3c, 0, 0);
    EnemyAttackParticle_SetGraphicsParameters(p, 0x848f, 0, -1);
    EnemyAttackParticle_SetAnimationParameters(p, 0, 0, 0);
    EnemyAttackParticle_SetUpdateCallback(p, EnemyAttackParticle_UpdateLifetime);
    EnemyAttackParticle_SetVerticalMotion(p, 0, 0);
    *(u32 *)(p + 0xc) = 0;
}
