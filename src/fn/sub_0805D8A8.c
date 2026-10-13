#include "global.h"
struct H {
    u32 lo : 16;
    u32 hi : 16;
};
struct Z {
    struct H a, b;
};
void Collider_Init(u8 *, u32, u32, u32, u32, struct Z *, struct Z *);
void sub_0821FF7C(u8 *, u32, u32, u32);
void Collider_SetCallback(u8 *, void (*)(void), u8 *);
void Collider_SetPosition(u8 *, u8 *, u32);
void Collider_Refresh(u8 *);
void sub_0805CCF0(void);
void sub_0805D8A8(u8 *p, u32 x)
{
    struct Z s1, s2;
    u8 *q = p + 0x68;
    s1.a.lo = 0x64;
    s1.a.hi = 0xc8;
    s1.b.lo = 0x64;
    *(u32 *)&s2.a = 0xc80000;
    *(u32 *)&s2.b &= 0xFFFF0000;
    Collider_Init(q, 0, 0x4001, 0, 0x10, &s1, &s2);
    sub_0821FF7C(q, x, 0, 0);
    Collider_SetPosition(q, p + 0x60, 0);
    Collider_SetCallback(q, sub_0805CCF0, p);
}
