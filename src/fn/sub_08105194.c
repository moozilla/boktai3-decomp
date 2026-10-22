#include "global.h"
struct H {
    u32 lo : 16;
    u32 hi : 16;
};
struct Z {
    struct H a, b;
};
void Collider_Init(u8 *, u32, u32, u32, u32, struct Z *, struct Z *);
void Collider_SetBodyProperties(u8 *, u32, u32, u32);
void Collider_SetPosition(u8 *, u8 *, u32);
void Collider_SetCallback(u8 *, void (*)(void), u8 *);
s32 Collider_Refresh(u8 *);
void CallbackPool_ApplyDamage(void);
void sub_08105194(u8 *s, u32 a, u32 b, u32 c)
{
    struct Z v, w;
    u8 *q = s + 0x58;
    w.a.lo = 0;
    w.a.hi = 0x64;
    w.b.lo = 0x40;
    v.a.lo = 0x40;
    v.a.hi = 0x3C;
    v.b.lo = 0x40;
    *(u16 *)(q + 4) = *(u32 *)(s + 0x140);
    Collider_Init(q, 0, 0x5001, 0, 0x10, &v, &w);
    Collider_SetBodyProperties(q, a, b, c);
    Collider_SetPosition(q, s + 0x1C, 0);
    Collider_SetCallback(q, CallbackPool_ApplyDamage, s);
    Collider_Refresh(q);
}
