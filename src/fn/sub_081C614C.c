#include "global.h"

struct S { u8 pad[0x370]; s32 f370; };
void sub_081C5F90(struct S *);

void sub_081C614C(struct S *p)
{
    p->f370++;
    if (p->f370 > 0x20) sub_081C5F90(p);
}
