#include "repeated_effect.h"
#define RepeatedEffect_TickManager sub_0804FF48
s32 RepeatedEffect_TickManager(struct RepeatedEffectManager *p)
{
    s32 i = 0;
    if (i < p->count) {
        struct RepeatedEffect *q = p->entries;
        do {
            sub_0804FC84(q);
            q++;
            i++;
        } while (i < p->count);
    }
    return 0;
}
