#include "global.h"

struct E { u8 act; u8 f[0x9f]; };
struct S { u8 f[0x38]; struct E e[8]; };
void sub_080125FC(void *, void *);

s32 sub_08012630(struct S *s)
{
    struct E *e = s->e;
    s32 i;
    for (i = 7; i >= 0; i--, e++) {
        if (e->act != 0)
            sub_080125FC(s, e);
    }
    return 0;
}
