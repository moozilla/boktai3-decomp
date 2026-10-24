#include "global.h"
struct H {
    u32 lo : 16;
    u32 hi : 16;
};
struct Tail {
    u32 lo : 16;
    u32 : 16;
};
struct Z {
    struct H a;
    struct Tail b;
};
void Collider_Init(u8 *, u32, u32, u32, u32, struct Z *, struct Z *);
void Collider_SetAttackProperties(u8 *, u32, u32, u32, u32, u32);
void Collider_SetCallback(u8 *, void (*)(void), u8 *);
void FragmentBurst_OnContact(void);
// confidence=low: sets up the fragment burst's attack collider at +0x100.
void sub_081281D0(u8 *p, u32 a, u32 b, u32 c, u32 d)
{
    struct Z s1, s2;
    u8 *q;
    s1.a.lo = 0x80;
    s1.a.hi = 0x20;
    s1.b.lo = 0x80;
    s2.a.lo = 0;
    s2.a.hi = 0;
    s2.b.lo = 0;
    q = p + 0x100;
    Collider_Init(q, 0, 0x2001, 0, 0x10, &s1, &s2);
    Collider_SetAttackProperties(q, a, b, 0, c, d);
    Collider_SetCallback(q, FragmentBurst_OnContact, p);
}
