#include "global.h"
struct S { u8 f[0x14]; u8 c; };
void sub_0811C99C(struct S *, u32);
void sub_0811C9D8(struct S *s)
{
    if (s->c != 0) {
        s->c--;
        if (s->c == 0) sub_0811C99C(s, 0);
    }
}
