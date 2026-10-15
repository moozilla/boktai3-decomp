#include "global.h"

struct Unk08065BEC {
    u32 unk0;
};

struct Unk08065BEC *ActorAttachment_AllocateSlot(void);

// &p->unk0 == p, but agbcc keeps the null test for the address expression
u32 *sub_08065C40(void)
{
    struct Unk08065BEC *p = ActorAttachment_AllocateSlot();
    if (!p)
        return NULL;
    return &p->unk0;
}
