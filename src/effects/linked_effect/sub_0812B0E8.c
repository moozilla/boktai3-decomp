#include "engine/shared/global.h"

struct Unk0812B084 {
    u32 unk0;
};

struct Unk0812B084 *LargeEffectPool_AllocateSlot(void *);

// &p->unk0 == p, but agbcc keeps the null test for the address expression
u32 *FragmentProjectilePool_AllocSprite(void *pool)
{
    struct Unk0812B084 *p = LargeEffectPool_AllocateSlot(pool);
    if (!p)
        return NULL;
    return &p->unk0;
}
