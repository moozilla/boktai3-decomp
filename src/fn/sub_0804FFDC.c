#include "repeated_effect.h"
s32 RepeatedEffect_InitManager(struct RepeatedEffectManager *manager)
{
    manager->count = 0;
    gRepeatedEffectManager = manager;
    return 0;
}
