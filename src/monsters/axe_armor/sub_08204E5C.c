#include "global.h"
#include "engine/video/display_animation.h"
struct SceneCountdownSequence {
    u8 unknown0[0x108];
    s32 (*update)(struct SceneCountdownSequence *);
    struct DisplaySequenceState sequence;
    u8 unknown11C[0x10];
    s16 countdown;
    u8 unknown12E[4];
    u8 variant;
    u8 unknown133[0x25];
    void *resource;
};
void AxeArmor_FireProjectile(struct SceneCountdownSequence *);
s32 AxeArmor_UpdateAttackTiming(struct SceneCountdownSequence *);
void Anim_RestartSequence(struct DisplaySequenceState *, void *, u32, u32, u32);
s32 AxeArmor_UpdateCountdown(struct SceneCountdownSequence *p)
{
    s16 *count = &p->countdown;
    if (*count > 0) {
        if (*count == 40)
            AxeArmor_FireProjectile(p);
        --*count;
    } else {
        Anim_RestartSequence(&p->sequence, p->resource, 2, p->variant, 0);
        p->update = AxeArmor_UpdateAttackTiming;
    }
    return 0;
}
