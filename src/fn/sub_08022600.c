#include "global.h"
u8 *sub_0802140C(int);
void SceneActors_AddResourceCost(int amount)
{
    u8 *p;
    if (amount > 0) {
        p = sub_0802140C(0);
        if (p)
            *(int *)(p + 0x39c) += amount;
    }
}
