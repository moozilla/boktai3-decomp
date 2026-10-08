#include "global.h"

struct E { u8 act; u8 f[0xeb]; };
struct S { u8 f[0x18]; u32 n; struct E *e; };

struct E *sub_08008A70(struct S *s)
{
    struct E *e = s->e;
    u32 i;
    for (i = 0; i < s->n; i++, e++) {
        if (e->act == 0)
            return e;
    }
    return 0;
}
