#include "global.h"

struct S { u8 f[0x1ba]; u16 t; };
void sub_0822B2F8(u32);
void sub_08055BE0(struct S *);
void sub_0821A0C0(struct S *);

void sub_08055D38(struct S *p)
{
    u16 *t = &p->t;
    if (*t == 0)
        sub_0822B2F8(0x4EC);
    (*t)++;
    if (*t > 0x59) {
        sub_08055BE0(p);
        sub_0821A0C0(p);
    }
}
