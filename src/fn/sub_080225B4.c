#include "global.h"
u8 *SceneActors_GetBySlot(int);
void SceneActors_RequestContactPoolClear(void);
void SceneActors_AddHealthCost(int amount)
{
    u8 *p;
    if (amount > 0) {
        p = SceneActors_GetBySlot(0);
        if (p) {
            int remaining, diff;
            *(int *)(p + 0x3a0) += amount;
            remaining = *(s16 *)(p + 0x94) - *(int *)(p + 0x3a0);
            if (remaining <= 1)
                goto stop;
            {
                diff = remaining - *(int *)(p + 0x3a4);
                if (diff < 0)
                    diff = -diff;
                if (diff > 49) {
                stop:
                    SceneActors_RequestContactPoolClear();
                }
            }
        }
    }
}
