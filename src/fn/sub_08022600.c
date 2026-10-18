#include "global.h"
u8 *SceneActors_GetBySlot(int);
void SceneActors_AddResourceCost(int amount)
{
    u8 *p;
    if (amount > 0) {
        p = SceneActors_GetBySlot(0);
        if (p)
            *(int *)(p + 0x39c) += amount;
    }
}
