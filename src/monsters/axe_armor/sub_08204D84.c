#include "monsters/axe_armor/axe_armor.h"

void Anim_RestartSequence(void *, void *, u32, u32, u32);
void Anim_SetDurationScale(void *, u32);
s32 Mod(s32, s32);
s32 AxeArmor_UpdateCountdown(struct AxeArmor *);

s32 AxeArmor_UpdateAttackTiming(struct AxeArmor *enemy)
{
    if (enemy->attackSchedule) {
        s16 *index = &enemy->attackIndex;
        s32 selected = *index * 2;
        s16 *table = enemy->attackTimes;
        if (enemy->timer > *(s16 *)((u8 *)table + selected)) {
            void *anim = &enemy->sequence;
            void *resource = enemy->resource;
            u32 facing = (u8)enemy->facing;
            Anim_RestartSequence(anim, resource, 3, facing, 0);
            Anim_SetDurationScale(anim, 64);
            enemy->update = AxeArmor_UpdateCountdown;
            enemy->countdown = 80;
            enemy->timer = 0;
            (*index)++;
            if (*index > enemy->attackCount - 1)
                *index = 0;
        }
    } else {
        s32 value = Mod(enemy->timer, enemy->interval);
        if (!value) {
            Anim_RestartSequence(&enemy->sequence, enemy->resource, 3, (u8)enemy->facing,
                                 value);
            enemy->update = AxeArmor_UpdateCountdown;
        }
    }
    return 0;
}
