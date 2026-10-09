#include "global.h"
struct H { u32 lo:16; u32 hi:16; };
struct Tail { u32 lo:16; u32 :16; };
struct Z { struct H a; struct Tail b; };
void sub_0821FEB4(u8 *, u32, u32, u32, u32, struct Z *, struct Z *);
void sub_0821FF58(u8 *, u32, u32, u32, u32, u32);
void sub_0821FF84(u8 *, void (*)(void), u8 *);
void sub_0821FF24(u8 *, u8 *, u32);
void sub_0812C310(void);
void sub_0812C314(u8 *p, u32 a, u32 b, u32 c, u32 d)
{
    struct Z s1, s2;
    u8 *q;
    s1.a.lo = 0x18;
    s1.a.hi = 0x80;
    s1.b.lo = 0x18;
    s2.a.lo = 0; s2.a.hi = 0;
    s2.b.lo = 0;
    q = p + 0x3C;
    sub_0821FEB4(q, 0, 0x2401, 0, 0x10, &s1, &s2);
    sub_0821FF58(q, a, b, 0, c, d);
    sub_0821FF84(q, sub_0812C310, p);
    sub_0821FF24(q, p + 0x1c, 0);
}
