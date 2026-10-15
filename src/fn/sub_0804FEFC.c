#include "repeated_effect.h"
#define RepeatedEffect_InitEntry sub_0804FEFC

void RepeatedEffect_InitEntry(struct RepeatedEffectManager *p, u32 a1,
                              struct RepeatedEffectPosition *a2, u32 a3, u32 a4, u32 a5, u32 a6)
{
    struct RepeatedEffect *e = &p->entries[p->count];
    sub_0804FD70(e, a1, a2, a3, a5, a6);
    sub_0804FDB4(e, a2, a4);
    sub_0804FE20(e);
    sub_0804FEA8(e);
}
