#include "global.h"

struct Unk0812A24C { u32 unk0; };

struct Unk0812A24C *sub_0812A24C(void);

// &p->unk0 == p, but agbcc keeps the null test for the address expression
u32 *sub_0812A2AC(void)
{
    struct Unk0812A24C *p = sub_0812A24C();
    if (!p)
        return NULL;
    return &p->unk0;
}
