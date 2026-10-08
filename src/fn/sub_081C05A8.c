#include "global.h"
struct S { u8 f[0x1c]; u32 a; u32 b; u32 c; u32 d; };
extern u8 gUnk_03004FC0[];
void sub_081C0478(void *, u32, u32, u32, u32, u32);
void sub_081C0538(void *);
void sub_081C0578(void *);
void sub_081C05A8(struct S *p)
{
    sub_081C0478(gUnk_03004FC0, p->a, p->b, p->c, p->d, 3);
    p->c = p->c + 1;
    sub_081C0538(p);
    if ((p->c >> p->d) != 0) sub_081C0578(p);
}
