#include "global.h"
struct E { u8 p[0xa]; u16 id; };
struct T { u8 n; u8 p[3]; struct E e[1]; };
struct A { u8 p[0xc]; struct T *fc; };
extern struct A *gUnk_030052F4;
struct E *sub_0821E104(u16 id, u16 *cnt)
{
    struct E *first;
    s32 i;
    *cnt = 0;
    first = 0;
    for (i = 0; i < gUnk_030052F4->fc->n; i++) {
        struct E *e = &gUnk_030052F4->fc->e[i];
        if (e->id == id) {
            if (first == 0) first = e;
            (*cnt)++;
        }
    }
    return first;
}
