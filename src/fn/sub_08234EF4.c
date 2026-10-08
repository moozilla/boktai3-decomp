#include "global.h"

struct E { u8 pad[0x3C]; };
struct S { u8 pad[0x8C]; struct E e[8]; };
void sub_082195E0(u8 *);
void sub_08217EAC(struct E *);

s32 sub_08234EF4(struct S *p)
{
    struct E *e;
    s32 i;
    sub_082195E0((u8 *)p + 0x1C);
    e = p->e;
    for (i = 7; i >= 0; i--) {
        sub_08217EAC(e);
        e++;
    }
    return 0;
}
