#include "global.h"
struct M { u32 a; u16 *b; };
s32 sub_0821A3E8(u32, struct M **);
void sub_0804F384(u8 *p)
{
    struct M *m;
    s32 n = sub_0821A3E8(*(u16 *)(p + 0x260), &m);
    s32 i = n;
    i--;
    if (n > 0) {
        u8 *t = p + 0x265;
        struct M *e = m;
        u32 one = 1;
        u32 zero = 0;
        do {
            switch (*e->b) {
            case 0: *t = one; break;
            case 1: *t = zero; break;
            }
            e++;
        } while (i-- > 0);
        m = e;
    }
}
