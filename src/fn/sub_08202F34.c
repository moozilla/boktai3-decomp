#include "global.h"
struct S { u8 p[0x10]; u32 w10; };
void sub_08202F34(struct S *p, u32 a)
{
    p->w10 = a;
}
