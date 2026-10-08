#include "global.h"
struct S { u8 f[0x30]; void *a; };
s32 sub_081C3010(void *);
void sub_081C08F4(void *);
void sub_081C08D8(struct S *p)
{
    if (sub_081C3010(p->a) != 0) sub_081C08F4(p);
}
