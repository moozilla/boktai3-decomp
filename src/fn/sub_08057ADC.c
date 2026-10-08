#include "global.h"

struct E { u8 f[0x34]; };
struct S { u8 f[0x564]; struct E e[32]; };
void sub_08217EAC(struct E *);

void sub_08057ADC(struct S *p)
{
    s32 i;
    struct E *e = p->e;
    for (i = 31; i >= 0; i--)
    {
        sub_08217EAC(e);
        e++;
    }
}
