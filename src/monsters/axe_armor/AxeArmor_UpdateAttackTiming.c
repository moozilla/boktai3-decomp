#include "monsters/axe_armor/axe_armor.h"

void Anim_RestartSequence(void *, void *, u32, u32, u32);
void Anim_SetDurationScale(void *, u32);
s32 Mod(s32, s32);
s32 AxeArmor_UpdateCountdown(struct AxeArmor *);

s32 AxeArmor_UpdateAttackTiming(struct AxeArmor *enemy)
{
    if (enemy->attackSchedule) {
        if (enemy->timer > enemy->attackTimes[enemy->attackIndex]) {
            Anim_RestartSequence(&enemy->sequence, enemy->resource, 3, ((u8)enemy->facing), 0);
            Anim_SetDurationScale(&enemy->sequence, 64);
            enemy->update = AxeArmor_UpdateCountdown;
            enemy->countdown = 80;
            enemy->timer = 0;
            enemy->attackIndex++;
            if (enemy->attackIndex > enemy->attackCount - 1)
                enemy->attackIndex = 0;
        }
    } else {
        s32 value = Mod(enemy->timer, enemy->interval);
        if (!value) {
            Anim_RestartSequence(&enemy->sequence, enemy->resource, 3, (u8)enemy->facing, value);
            enemy->update = AxeArmor_UpdateCountdown;
        }
    }
    return 0;
}
