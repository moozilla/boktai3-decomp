#include "global.h"

struct S { u8 f[0x382]; u16 t; };
void sub_08052628(struct S *, void (*)(void));
void sub_08052D60(void);

void sub_08052D34(struct S *p)
{
    p->t++;
    if (p->t > 0xf)
        sub_08052628(p, sub_08052D60);
}
