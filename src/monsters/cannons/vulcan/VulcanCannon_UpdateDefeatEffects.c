#include "monsters/cannons/vulcan_cannon_worker.h"
void TrapSpellParticles_CreateDefaultBurst(void *, s32, s32, s32, s32, s32, s32, s32, s32, s32,
                                             s32, s32);
void VulcanCannon_RecordDefeat(void *);
s32 VulcanCannon_UpdateDefeatEffects(struct VulcanCannon *p)
{
    VulcanCannon_StepAnimation(p);
    if (!p->inert) {
        if ((u32)(p->timer - 13) <= 10) {
            struct ContactPosition position;
            u32 random;
            VulcanCannon_GetEffectPosition(&position, p);
            gRandomIndex = (gRandomIndex + 1) & 1023;
            random = *(const u8 *)((u32)gRandomTable + gRandomIndex * 2);
            TrapSpellParticles_CreateDefaultBurst(&position, 128, 32, 12, 12, 24, random, 32, 32,
                                                    1, 2, 512);
        }
        if (p->timer == 1)
            VulcanCannon_RecordDefeat(p);
    }
    return 0;
}
