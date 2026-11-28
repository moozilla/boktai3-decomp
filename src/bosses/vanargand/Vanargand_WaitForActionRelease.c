#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"

s32 Player_IsSlotInActionTwelveOrThirtyEight(s32);
void Vanargand_SetPendingCue(void *, u16);
void Vanargand_WaitForActionRelease(struct VanargandPairAttack *p)
{
    struct VanargandPairAttack *actor = p;
    u32 flag = 4;
    p->flags |= flag;
    Vanargand_SetPendingCue(p, 3);
    if (p->pending) {
        p->pending = 0;
        if (p->transition <= 2) {
            u16 delay = 6;
            p->paletteSelector = 0;
            p->paletteMode = delay;
            p->paletteShift = delay;
            p->paletteFrame = 0;
        }
        p->phase = 0;
        p->timer = 0;
    }
    if (!p->phase && !actor->paletteMode) {
        s32 busy = Player_IsSlotInActionTwelveOrThirtyEight(0);
        if (!busy) {
            p->finished = 1;
            p->phase = 1;
            p->timer = busy;
        }
    }
}
