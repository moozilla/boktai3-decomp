#include "global.h"

struct S { u8 f[0x184]; u8 a; };
void sub_08042558(void *);
void sub_0802D43C(struct S *p)
{
    sub_08042558(&p->a);
}
