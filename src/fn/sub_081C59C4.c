#include "global.h"
struct E { u8 pad[0x60]; };
struct S { u8 f[0x78]; struct E e[4]; };
void sub_082195E0(void *);
void sub_082156B8(u32);
void sub_081C59C4(struct S *p)
{
    s32 i;
    struct E *e = p->e;
    for (i = 3; i >= 0; i--) { sub_082195E0(e); e++; }
    sub_082195E0((u8 *)p + 0x1f8);
    sub_082195E0((u8 *)p + 0x258);
    sub_082156B8(0);
    sub_082156B8(1);
    sub_082156B8(2);
    sub_082156B8(3);
}
