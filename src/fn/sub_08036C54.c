#include "global.h"

struct S { u8 f[0x1d]; u8 a; };
extern struct S *gUnk_02000484;
void sub_08036C54(void)
{
    struct S *p = gUnk_02000484;
    if (p) p->a = 1;
}
