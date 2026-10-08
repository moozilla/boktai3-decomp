#include "global.h"
struct S { u8 f[0x30]; void *a; };
s32 sub_081C3010(void *);
void sub_081C09BC(void *);
void sub_081C09A0(struct S *p)
{
    if (sub_081C3010(p->a) != 0) sub_081C09BC(p);
}
