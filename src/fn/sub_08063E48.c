#include "global.h"
struct H {
    u32 lo : 16;
    u32 hi : 16;
};
struct Z {
    struct H a, b;
};
void Collider_Init(u8 *, u32, u32, u32, u32, struct Z *, struct Z *);
void sub_0821FF58(u8 *, u32, u32, u32, u32, u32);
void Collider_SetCallback(u8 *, u32, u8 *);
void sub_08063E48(u8 *p)
{
    struct Z s1, s2;
    u8 *q = p + 0xbc;
    u32 k;
    s1.a.lo = 0x40;
    s1.a.hi = 0x64;
    s1.b.lo = 0x40;
    *(u32 *)&s2.a = 0x640000;
    *(u32 *)&s2.b &= 0xFFFF0000;
    Collider_Init(q, 0, 0x2001, 0, k = 0x10, &s1, &s2);
    sub_0821FF58(q, 0xa, 0x1e, 0, 0, k);
    Collider_SetCallback(q, 0, p);
}
