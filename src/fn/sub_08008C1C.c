#include "global.h"
struct T { u16 a, b; };
extern const struct T gUnk_086049CE[];
extern const u16 gUnk_086049DA[];
struct S { u8 f[0x18]; s32 idx; u8 i[0x1c]; u8 q[0xb0]; u32 e8; };
s32 sub_08008AA4(void *);
void sub_08220D78(void *, u32, u32, u32, u32);
void sub_08219A80(void *, u32);

void sub_08008C1C(u32 unused, struct S *s)
{
    if (sub_08008AA4(s)) {
        void *q = (u8 *)s + 0x34;
        sub_08220D78(q, s->e8, gUnk_086049CE[s->idx].a, 1, 0);
        if (s->idx != 2)
            sub_08219A80(q, gUnk_086049DA[s->idx] + 1);
        else
            sub_08219A80(q, gUnk_086049DA[2]);
    }
}
