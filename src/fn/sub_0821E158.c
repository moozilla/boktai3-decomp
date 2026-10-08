#include "global.h"
struct E { u32 id; u8 p[0x28]; };
struct T { u8 n; };
struct A { u8 p[0xc]; struct T *fc; u8 q[0x214]; struct E e[1]; };
extern struct A *gUnk_030052F4;
struct E *sub_0821E158(u32 id)
{
    s32 i = 0;
    struct A *a = gUnk_030052F4;
    if (i < a->fc->n) {
        s32 n = a->fc->n;
        struct E *e = a->e;
        do {
            if (e->id == id) return e;
            e++;
            i++;
        } while (i < n);
    }
    return 0;
}
