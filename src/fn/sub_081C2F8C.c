#include "global.h"
struct S { u8 f[0x770]; u32 a770; u8 g[8]; void (*a77c)(void *); };
extern struct S *gUnk_02000264;
void sub_0822B180(u32);
void sub_081C2BE8(void *);
void sub_081C2F8C(void)
{
    struct S *p = gUnk_02000264;
    if (p != 0) {
        void (*f)(void *);
        sub_0822B180(8);
        f = sub_081C2BE8;
        p->a77c = f;
        p->a770 = 0;
    }
}
