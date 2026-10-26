// MATCHING_CONCESSION: register-pinning
#include "vulcan_cannon_worker.h"
void SpriteParticle_CreateDefaultVariant4384(void *, s32, s32, s32, s32, s32, s32, s32, s32, s32,
                                             s32, s32);
void VulcanCannon_RecordDefeat(void *);
static inline void CopyPosition(struct SceneVec *out, struct SceneVec *in)
{
    *out = *in;
}
static inline void RaisePosition(struct SceneVec *v)
{
    v->y += 128;
}
s32 VulcanCannon_UpdateDefeatEffects(struct VulcanCannon *p)
{
    VulcanCannon_StepAnimation(p);
    if (!p->inert) {
        if ((u32)(p->timer - 13) <= 10) {
            struct SceneVec position;
            u32 random;
            CopyPosition(&position, &p->display.spatial.position);
            RaisePosition(&position);
            gRandomIndex = (gRandomIndex + 1) & 1023;
            random = *(const u8 *)((u32)gUnk_0203B400 + gRandomIndex * 2);
            SpriteParticle_CreateDefaultVariant4384(&position, 128, 32, 12, 12, 24, random, 32, 32,
                                                    1, 2, 512);
        }
        if (p->timer == 1)
            VulcanCannon_RecordDefeat(p);
    }
    return 0;
}
