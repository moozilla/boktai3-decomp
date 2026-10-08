#include "global.h"

struct E { u8 pad[0x48]; };
struct S { u8 pad[0x40]; struct E e[12]; };
void sub_08214514(struct E *);

void sub_081D3078(struct S *p)
{
    struct E *e = p->e;
    s32 i;
    for (i = 11; i >= 0; i--) {
        sub_08214514(e);
        e++;
    }
}
