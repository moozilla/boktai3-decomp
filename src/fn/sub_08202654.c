#include "global.h"
struct S { u32 w0; u32 w4; u8 p[0x2a - 8]; u16 h2a; };
void sub_08202F48(void *);
u32 sub_08202654(struct S *p)
{

    { u32 m = 2; p->w4 = p->w4 | m; }
    sub_08202F48(p);
    if (p->w0 > p->h2a)
        return 1;
    return 0;
}
