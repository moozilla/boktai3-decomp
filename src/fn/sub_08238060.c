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
void Collider_SetPosition(u8 *, u8 *, u32);
void Collider_Refresh(u8 *);
void SpritePulseEffect_OnAnimationEvent(void);
#define SpritePulseEffect_InitCollider sub_08238060
void SpritePulseEffect_InitCollider(u8 *p, u32 type, u32 a, u32 b, u32 c, u32 d)
{
    struct Z s1, s2;
    u8 *q = p + 0x60;
    s1.a.lo = 0x1e;
    s1.a.hi = 0x1e;
    s1.b.lo = 0x1e;
    s2.a.lo = 0;
    s2.a.hi = 0;
    s2.b.lo = 0;
    Collider_Init(q, 0, 0x2101, 0, 1, &s1, &s2);
    Collider_SetAttackProperties(q, a, b, 0x10, c, d);
    Collider_SetCallback(q, SpritePulseEffect_OnAnimationEvent, p);
    Collider_SetPosition(q, p + 0x34, 0);
    p[0xa2] = type;
    Collider_Refresh(q);
}
