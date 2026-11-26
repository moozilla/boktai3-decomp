#include "monsters/ooze.h"
s32 Mod(s32, s32);
void Ooze_BeginAttack(void);
s32 Ooze_UpdateAttackDelay(struct Ooze *p)
{
    if (p->delayScript) {
        if (p->dispatchTicks > p->delays[p->delayIndex]) {
            p->state = Ooze_BeginAttack;
            p->dispatchTicks = 0;
            p->delayIndex++;
            if (p->delayIndex > p->delayCount - 1)
                p->delayIndex = 0;
        }
    } else {
        if (Mod(p->dispatchTicks, p->interval) == 0)
            p->state = Ooze_BeginAttack;
    }
    return 0;
}
