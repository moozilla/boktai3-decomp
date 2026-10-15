#include "repeated_effect.h"
s32 RepeatedEffect_InitManager(struct RepeatedEffectManager *manager)
{
    manager->count = 0;
    gUnk_02000110 = manager;
    return 0;
}
