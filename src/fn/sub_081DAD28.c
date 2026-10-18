#include "global.h"
#include "task4e01.h"
void TimedEffect_RemoveFromManager(struct Task4E01 *, struct Task4E01Entry *, s32);
s32 Task4E01_RemoveEntry(s32 i)
{
    struct Task4E01 *p = ResourceRegistry_GetResource(0x4e01);
    struct Task4E01Entry *e;
    if (!p || i < 0 || i >= p->count || !(p->activeMask & (1 << i)))
        return 1;
    e = &p->entries[i];
    if (e->kind == 3 && (e->flags & 0x10))
        --*(u16 *)(e->owner + 0x50);
    TimedEffect_RemoveFromManager(p, e, i);
    return 0;
}
