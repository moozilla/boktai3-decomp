#include "global.h"

struct Unk08121848 { u32 unk0; };

struct Unk08121848 *sub_08121848(void);

// &p->unk0 == p, but agbcc keeps the null test for the address expression
u32 *sub_08121890(void)
{
    struct Unk08121848 *p = sub_08121848();
    if (!p)
        return NULL;
    return &p->unk0;
}
