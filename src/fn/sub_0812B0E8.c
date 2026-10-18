#include "global.h"

struct Unk0812B084 {
    u32 unk0;
};

struct Unk0812B084 *sub_0812B084(void);

// &p->unk0 == p, but agbcc keeps the null test for the address expression
u32 *FragmentProjectilePool_AllocSprite(void)
{
    struct Unk0812B084 *p = sub_0812B084();
    if (!p)
        return NULL;
    return &p->unk0;
}
