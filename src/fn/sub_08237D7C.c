#include "global.h"

struct E { u8 pad[0x30]; };
struct S { u8 pad[0x7c]; struct E e[8]; };
void sub_08214514(u8 *);
void sub_08217EAC(struct E *);

s32 sub_08237D7C(struct S *p)
{
    struct E *e;
    s32 i;
    sub_08214514((u8 *)p + 0x1C);
    e = p->e;
    for (i = 3; i >= 0; i--) {
        sub_08217EAC(e);
        e++;
    }
    return 0;
}
