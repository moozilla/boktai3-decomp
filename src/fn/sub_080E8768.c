#include "global.h"

extern const u8 gUnk_086062AC[];

struct S {
    u8 filler[0x288];
    const u8 *p;
};

void Mimic_InitActionTable(struct S *s)
{
    s->p = gUnk_086062AC;
}
