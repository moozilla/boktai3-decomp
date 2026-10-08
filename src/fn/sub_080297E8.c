#include "global.h"

struct S { u8 f[0x888]; u8 a; };
void sub_08214514(void *);
void sub_080297E8(struct S *p)
{
    sub_08214514(&p->a);
}
