#include "global.h"

struct S { u8 f[3]; u8 a; };
void sub_08020D28(void);
u32 sub_08020D68(struct S *p, u32 v)
{
    u32 r;
    if (p->a != v) r = 0;
    else {
        sub_08020D28();
        p->a = 0;
        r = 1;
    }
    return r;
}
