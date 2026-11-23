#include "global.h"

struct Unk0812A24C {
    u32 unk0;
};

struct Unk0812A24C *EffectPoolA_AllocateSlot(void *);

// &p->unk0 == p, but agbcc keeps the null test for the address expression
u32 *SunDiscPool_AllocSprite(void *pool)
{
    struct Unk0812A24C *p = EffectPoolA_AllocateSlot(pool);
    if (!p)
        return NULL;
    return &p->unk0;
}
