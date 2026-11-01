#include "vulcan_cannon_worker.h"
s32 Mod(s32, s32);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void VulcanCannon_UpdateTargeting(void);
void VulcanCannon_FireProjectile(void *);
s32 VulcanCannon_UpdateFiringBurst(struct VulcanCannon *p)
{
    if (Mod(p->timer, 10) == 0 && p->timer <= 30) {
        Anim_RestartSequence(&p->sequence, p->resource, 0x12, (u8)p->facing, 8);
        if (Mod(p->shotCount, 3) == 0)
            Sound_PlaySong(0x51d);
        VulcanCannon_FireProjectile(p);
        p->shotCount++;
    }
    if (p->timer > p->burstDelay) {
        p->timer = 0;
        if (p->shotCount >= p->burstCount * 3 && p->burstCount != 0) {
            Anim_RestartSequence(&p->sequence, p->resource, 0x11, (u8)p->facing, 0);
            p->state = VulcanCannon_UpdateTargeting;
            p->timer = 0;
        }
    }
    VulcanCannon_StepAnimation(p);
    return 0;
}
