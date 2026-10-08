#include "global.h"

struct E { u16 id; u8 f[2]; u8 on; u8 g[0x37]; };
struct S { u8 f[0x1c]; struct E e[8]; };
struct E *sub_08019818(struct S *p, u32 id)
{
    struct E *e = p->e;
    s32 i = 0;
    do {
        if (e->on != 0 && e->id == id)
            return e;
        i++;
        e++;
    } while (i <= 7);
    return 0;
}
