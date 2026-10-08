#include "global.h"

struct S { u8 pad[0x310]; s32 f310; };
void sub_081C55D8(struct S *, void (*)(void));
void sub_081C5894(void);

void sub_081C57E8(struct S *p)
{
    p->f310++;
    if (p->f310 > 0x20) sub_081C55D8(p, sub_081C5894);
}
