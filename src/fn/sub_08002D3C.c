#include "global.h"

struct S { u8 f0; u8 a; u8 b; u8 c; };
void sub_08002D3C(struct S *p)
{
    if (p->a > 1) {
        p->a -= 2;
    } else {
        p->a = 0;
        p->c = 0;
    }
}
