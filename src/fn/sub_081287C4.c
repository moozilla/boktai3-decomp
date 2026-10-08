#include "global.h"

struct Unk0812876C { u32 unk0; };

struct Unk0812876C *sub_0812876C(void);

// &p->unk0 == p, but agbcc keeps the null test for the address expression
u32 *sub_081287C4(void)
{
    struct Unk0812876C *p = sub_0812876C();
    if (!p)
        return NULL;
    return &p->unk0;
}
