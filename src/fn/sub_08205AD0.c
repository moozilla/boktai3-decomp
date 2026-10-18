#include "global.h"
#define Ooze_UpdateAttackDelay sub_08205AD0
#define Ooze_BeginAttack       sub_08205B70
struct Ooze {
    u8 pad0[0x104];
    void (*state)(void);
    u8 pad108[0x18];
    s16 interval, variant, delayCount, delayIndex;
    s16 delays[13];
    s16 timer;
    u8 pad144[0x18];
    u32 useDelayTable;
};
s32 Mod(s32, s32);
void sub_08205B70(void);
s32 sub_08205AD0(struct Ooze *p)
{
    if (p->useDelayTable) {
        if (p->timer > p->delays[p->delayIndex]) {
            p->state = Ooze_BeginAttack;
            p->timer = 0;
            p->delayIndex++;
            if (p->delayIndex > p->delayCount - 1)
                p->delayIndex = 0;
        }
    } else {
        if (Mod(p->timer, p->interval) == 0)
            p->state = Ooze_BeginAttack;
    }
    return 0;
}
