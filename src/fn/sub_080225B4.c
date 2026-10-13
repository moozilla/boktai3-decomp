#include "global.h"
#define SceneActors_AddHealthCost sub_080225B4
u8 *sub_0802140C(int);
void sub_08022590(void);
void SceneActors_AddHealthCost(int amount)
{
    u8 *p;
    if (amount > 0) {
        p = sub_0802140C(0);
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
                    sub_08022590();
                }
            }
        }
    }
}
