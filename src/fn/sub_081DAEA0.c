#include "global.h"
#include "task4e01.h"
#define Task4E01_GetKind7Value sub_081DAEA0
s32 sub_081DAEA0(s32 i)
{
    struct Task4E01 *p = ResourceRegistry_GetResource(0x4e01);
    struct Task4E01Entry *e;
    if (p && i >= 0 && i < p->count) {
        e = &p->entries[i];
        if (e->kind == 7)
            return e->value1A4;
    }
    return 0;
}
