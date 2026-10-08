#include "global.h"

struct S { u8 f[0x2e]; u8 a; };
struct S *sub_08037DC0(void);
void sub_080396F0(void)
{
    struct S *p = sub_08037DC0();
    if (p) p->a = 1;
}
