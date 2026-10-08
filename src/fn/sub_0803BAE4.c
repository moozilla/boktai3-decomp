#include "global.h"

struct S { u8 f[0x600]; u8 a; };
void sub_0811BD30(void *, void *);
void sub_0803BAE4(struct S *p)
{
    sub_0811BD30(p, &p->a);
}
