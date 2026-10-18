#include "global.h"

struct Unk0812876C {
    u32 unk0;
};

struct Unk0812876C *FragmentBurst_AllocateSlot(void);

// &p->unk0 == p, but agbcc keeps the null test for the address expression
u32 *FragmentBurst_AllocateSpriteGroup(void)
{
    struct Unk0812876C *p = FragmentBurst_AllocateSlot();
    if (!p)
        return NULL;
    return &p->unk0;
}
