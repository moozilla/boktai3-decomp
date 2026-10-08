#include "global.h"

struct E { u8 pad[0x30]; };
struct S { u8 pad[0x7c]; struct E e[8]; };
void sub_08214514(u8 *);
void sub_08217EAC(struct E *);

s32 sub_081D15C8(struct S *p)
{
    struct E *e;
    s32 i;
    sub_08214514((u8 *)p + 0x18);
    e = p->e;
    for (i = 7; i >= 0; i--) {
        sub_08217EAC(e);
        e++;
    }
    return 0;
}
