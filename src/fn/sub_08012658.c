#include "global.h"

struct E { u8 act; u8 f[0x9f]; };
struct S { u8 f[0x38]; struct E e[8]; };
extern u32 gUnk_02000050;
void sub_08012620(void *, void *);

u32 sub_08012658(struct S *s)
{
    struct E *e = s->e;
    s32 i;
    for (i = 7; i >= 0; i--, e++) {
        sub_08012620(s, e);
    }
    return gUnk_02000050 = 0;
}
