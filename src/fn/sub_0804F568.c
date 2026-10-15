#include "global.h"
#define ResourceStation_InitializeCollider sub_0804F568
struct H {
    u32 lo : 16;
    u32 hi : 16;
};
struct Z {
    struct H a, b;
};
void Collider_Init(u8 *, u32, u32, u32, u32, struct Z *, struct Z *);
void sub_0821FF7C(u8 *, u32, u32, u32);
void Collider_SetCallback(u8 *, u32, u8 *);
void Collider_SetPosition(u8 *, u8 *, u32);
s32 Collider_Refresh(u8 *);
s32 ResourceStation_InitializeCollider(u8 *p)
{
    struct Z s1, s2;
    u8 *q = p + 0x200;
    s1.a.lo = 0x64;
    s1.a.hi = 0x96;
    s1.b.lo = 0x64;
    *(u32 *)&s2.a = 0x960000;
    *(u32 *)&s2.b &= 0xFFFF0000;
    Collider_Init(q, *(u16 *)(p + 0x260), 0x4001, 0, 0, &s1, &s2);
    sub_0821FF7C(q, 0, 0, 0);
    Collider_SetCallback(q, 0, p);
    Collider_SetPosition(q, p + 0x34, 0);
    return Collider_Refresh(q);
}
