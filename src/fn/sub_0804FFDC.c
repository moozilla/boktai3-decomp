#include "repeated_effect.h"
#define RepeatedEffect_InitManager sub_0804FFDC
s32 RepeatedEffect_InitManager(struct RepeatedEffectManager *manager)
{
    manager->count = 0;
    gUnk_02000110 = manager;
    return 0;
}
