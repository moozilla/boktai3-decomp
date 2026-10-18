#include "global.h"

struct Unk08127EFC {
    u32 unk0;
};

struct Unk08127EFC *DirectionalProjectile_AllocateSlot(void);

// &p->unk0 == p, but agbcc keeps the null test for the address expression
u32 *DirectionalProjectile_AllocateSprite(void)
{
    struct Unk08127EFC *p = DirectionalProjectile_AllocateSlot();
    if (!p)
        return NULL;
    return &p->unk0;
}
