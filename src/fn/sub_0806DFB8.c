#include "global.h"

struct Unk0806DF60 { u32 unk0; };

struct Unk0806DF60 *sub_0806DF60(void);

// &p->unk0 == p, but agbcc keeps the null test for the address expression
u32 *sub_0806DFB8(void)
{
    struct Unk0806DF60 *p = sub_0806DF60();
    if (!p)
        return NULL;
    return &p->unk0;
}
