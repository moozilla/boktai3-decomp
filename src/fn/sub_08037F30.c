#include "global.h"

struct S { u8 f[0x54]; u8 a; u8 b; };
struct S *sub_08037DC0(void);
void sub_08036B64(u8 *);
void sub_08037F30(void)
{
    struct S *p = sub_08037DC0();
    if (p && p->b != 0)
        sub_08036B64(&p->a);
}
