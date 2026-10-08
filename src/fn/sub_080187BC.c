#include "global.h"
struct S { u8 f[0x24]; u16 n; };
u32 sub_080187BC(struct S *s)
{
    s32 i = 0;
    u8 *e;
    if (i < s->n) {
        s32 n = s->n;
        e = (u8 *)s + 0x34;
        do {
            if (*e == 1)
                return 1;
            e += 0x30;
            i++;
        } while (i < n);
    }
    return 0;
}
