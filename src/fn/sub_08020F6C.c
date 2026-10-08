#include "global.h"

struct S { u8 f[0x58]; u32 a; };
void sub_08018D84(u32);
void sub_08020F6C(struct S *p)
{
    if (p->a != 0) {
        sub_08018D84(p->a);
        p->a = 0;
    }
}
