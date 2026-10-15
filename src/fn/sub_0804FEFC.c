#include "repeated_effect.h"

void RepeatedEffect_InitEntry(struct RepeatedEffectManager *p, u32 a1,
                              struct RepeatedEffectPosition *a2, u32 a3, u32 a4, u32 a5, u32 a6)
{
    struct RepeatedEffect *e = &p->entries[p->count];
    RepeatedEffect_InitParameters(e, a1, a2, a3, a5, a6);
    RepeatedEffect_InitSprite(e, a2, a4);
    RepeatedEffect_InitCollider(e);
    RepeatedEffect_InitState(e);
}
