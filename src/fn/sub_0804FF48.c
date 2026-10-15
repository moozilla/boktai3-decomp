#include "repeated_effect.h"
s32 RepeatedEffect_TickManager(struct RepeatedEffectManager *p)
{
    s32 i = 0;
    if (i < p->count) {
        struct RepeatedEffect *q = p->entries;
        do {
            RepeatedEffect_TickEntry(q);
            q++;
            i++;
        } while (i < p->count);
    }
    return 0;
}
