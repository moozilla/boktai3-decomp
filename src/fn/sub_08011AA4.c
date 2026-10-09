#include "global.h"

struct E { u8 act; u8 f[0xb3]; };
struct S { u8 f[0x8C]; struct E e[16]; };
extern u32 gUnk_0200004C;
void sub_08011A18(void *, void *);

u32 sub_08011AA4(struct S *s)
{
    struct E *e = s->e;
    s32 i;
    for (i = 15; i >= 0; i--, e++) {
        sub_08011A18(s, e);
    }
    return gUnk_0200004C = 0;
}
