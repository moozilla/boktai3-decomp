#include "global.h"
#define Task4E01_RemoveEntry  sub_081DAD28
#define Task4E01_ReleaseEntry sub_081DA178
#include "task4e01.h"
void sub_081DA178(struct Task4E01 *, struct Task4E01Entry *, s32);
s32 sub_081DAD28(s32 i)
{
    struct Task4E01 *p = ResourceRegistry_GetResource(0x4e01);
    struct Task4E01Entry *e;
    if (!p || i < 0 || i >= p->count || !(p->activeMask & (1 << i)))
        return 1;
    e = &p->entries[i];
    if (e->kind == 3 && (e->flags & 0x10))
        --*(u16 *)(e->owner + 0x50);
    sub_081DA178(p, e, i);
    return 0;
}
