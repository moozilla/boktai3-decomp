#include "global.h"

struct S080FC844 { u8 filler[0x3d0]; u8 *q; };
void sub_0821FE6C(u8 *);
void sub_08013684(u8 *);
void sub_08214514(u8 *);

void sub_080FC844(struct S080FC844 *s)
{
    u8 *q = s->q;
    if (q[0xa3c]) {
        u8 *p = q + 0xb8c;
        s32 i = 2;
        do {
            sub_0821FE6C(p);
            p += 0x54;
        } while (--i >= 0);
        sub_08013684(q + 0xcac);
        sub_08214514(q + 0xa38);
    }
}
