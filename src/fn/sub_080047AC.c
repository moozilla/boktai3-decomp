#include "global.h"

struct S { u8 f0; u8 f1; u8 g[0x1c]; u8 f1e; u8 h[0xd]; u8 a[0x60]; u8 b[0x10]; u8 c[0x10]; u8 d[1]; };
void sub_08214514(void *);
void sub_0821D6D0(void *);
void sub_0821FE6C(void *);
void sub_08003C10(void *);
void sub_08225938(void *);

u32 sub_080047AC(struct S *s)
{
    sub_08214514((u8 *)s + 0x90);
    sub_0821D6D0((u8 *)s + 0xbc);
    sub_0821FE6C((u8 *)s + 0xcc);
    if ((s->f1 & 2) == 0)
        sub_08003C10((u8 *)s + 0x2c);
    sub_08225938((u8 *)s + 0x2c);
    return s->f1e = 0;
}
