#include "monsters/axe_armor/axe_armor.h"
#include "engine/video/display_animation.h"
void AxeArmor_FireProjectile(struct AxeArmor *);
s32 AxeArmor_UpdateAttackTiming(struct AxeArmor *);
void Anim_RestartSequence(struct DisplaySequenceState *, void *, u32, u32, u32);
s32 AxeArmor_UpdateCountdown(struct AxeArmor *p)
{
    s16 *count = &p->countdown;
    if (*count > 0) {
        if (*count == 40)
            AxeArmor_FireProjectile(p);
        --*count;
    } else {
        Anim_RestartSequence(&p->sequence, p->resource, 2, (u8)p->facing, 0);
        p->update = AxeArmor_UpdateAttackTiming;
    }
    return 0;
}
