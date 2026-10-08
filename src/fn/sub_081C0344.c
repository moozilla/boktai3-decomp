#include "global.h"
struct S { u8 f[0x20]; u32 fl; u8 g[0x80]; u32 a4; };
extern u32 gUnk_0300523C;
u32 sub_0804A138(void);
void sub_081C01D8(void *);
void sub_081C0344(struct S *p)
{
    if (gUnk_0300523C != 0) {
        p->fl |= 1;
    } else {
        p->a4 = sub_0804A138();
        sub_081C01D8(p);
    }
}
