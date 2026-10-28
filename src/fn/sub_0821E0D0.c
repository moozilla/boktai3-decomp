#include "global.h"
#include "ram.h"
struct E {
    u8 p[0xa];
    u16 id;
};
struct T {
    u8 n;
    u8 p[3];
    struct E e[1];
};
struct A {
    u8 p[0xc];
    struct T *fc;
};
u32 sub_0821E0D0(u16 id)
{
    s32 i = 0;
    struct T *t = ((struct A *)gTerrainMap)->fc;
    if (i < t->n) {
        s32 n = t->n;
        u8 *p = (u8 *)t;
        do {
            if (((struct E *)(p + 4))->id == id)
                return 1;
            p += 12;
            i++;
        } while (i < n);
    }
    return 0;
}
