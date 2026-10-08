#include "global.h"

struct Unk08127EFC { u32 unk0; };

struct Unk08127EFC *sub_08127EFC(void);

// &p->unk0 == p, but agbcc keeps the null test for the address expression
u32 *sub_08127F4C(void)
{
    struct Unk08127EFC *p = sub_08127EFC();
    if (!p)
        return NULL;
    return &p->unk0;
}
