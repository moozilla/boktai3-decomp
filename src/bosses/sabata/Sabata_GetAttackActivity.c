#include "bosses/sabata/sabata_attack_state.h"

s32 Sabata_GetAttackActivity(struct SabataAttackActor *volume)
{
    s32 result;
    if (volume->activityTimer > 0) {
    active:
        return 1;
    }
    {
        u32 mask = 1;
        if (mask & volume->flags) {
            if (volume->activity == 0)
                goto active;
            result = 2;
        } else {
            result = 0;
        }
    }
    return result;
}
