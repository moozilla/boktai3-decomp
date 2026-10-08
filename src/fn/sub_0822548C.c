#include "global.h"
struct S { u8 pad[0xc]; u32 fc; u32 f10; u16 f14; u16 f16; };
extern struct S *gUnk_030053F8;
void sub_0822548C(void)
{
    struct S *p = gUnk_030053F8;
    if (p) {
        p->fc = 0;
        p->f10 = 0;
        p->f14 = 0;
        p->f16 = 0;
    }
}
