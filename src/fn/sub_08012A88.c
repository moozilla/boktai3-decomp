#include "global.h"

struct E { u8 act; u8 st; u8 f[0x9e]; };
struct S { u8 f[0x38]; struct E e[8]; };
struct S *sub_08012040(void);
struct S *sub_080126DC(void);
void sub_0801204C(void *, void *, u32);

s32 sub_08012A88(void)
{
    struct S *s = sub_08012040();
    struct E *e;
    s32 i;
    if (s == 0) {
        s = sub_080126DC();
        if (s == 0)
            return -1;
    }
    e = s->e;
    for (i = 7; i >= 0; i--) {
        if (e->act != 0 && e->st <= 2)
            sub_0801204C(s, e, 3);
        e++;
    }
    return 0;
}
