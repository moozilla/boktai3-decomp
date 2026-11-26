#include "engine/shared/global.h"
#include "engine/shared/input.h"
#include "bosses/vanargand/vanargand_pair_attack.h"

void Vanargand_CycleDebugAnimation(struct VanargandPairAttack *p)
{
    if (gPadState[1] & 4) {
        s32 animation = (p->state + 1) % 32;
        if (animation == 0)
            animation = 1;
        Vanargand_EnterAction(p, animation);
    }
}
