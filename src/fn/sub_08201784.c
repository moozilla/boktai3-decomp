#include "global.h"
#ifdef NONMATCHING
// fuzzy: 63.1%
// 23 differing instruction runs; first diff at +0xA (mov sb, r0 vs adds r7, r0, #0)
struct H {
    u32 lo : 16;
    u32 hi : 16;
};
struct Z {
    struct H a, b;
};
struct Q {
    u8 pad[6];
    u16 flags;
};
void Collider_Init(u8 *, u16, u32, u32, u32, struct Z *, struct Z *);
void Collider_SetCallback(u8 *, void (*)(void), u8 *);
void sub_0821FF58(u8 *, s32, s32, u32, u32, s32);
void Collider_SetPosition(u8 *, u8 *, u32);
void sub_0821FF7C(void *, u32, u32, u32);
void Collider_Refresh(u8 *);
void sub_082015D0(void);
void sub_082015D4(void);
void sub_08201784(u8 *p)
{
    struct Z a, b;
    u8 *q = p + 0x6c;
    u16 *v, *r;
    a.a.lo = 120;
    a.a.hi = 120;
    a.b.lo = 120;
    *(u32 *)&b.a = 0x780000;
    b.b.lo = 0;
    Collider_Init(q, 0, 0x2001, 0, 0x10, &a, &b);
    Collider_SetCallback(q, sub_082015D0, p);
    sub_0821FF58(q, *(s16 *)(p + 0x174), *(s16 *)(p + 0x176), 0, 0, *(s16 *)(p + 0x178));
    q += 0x54;
    v = (u16 *)&a;
    r = (u16 *)&b;
    v[0] = 100;
    v[1] = 200;
    v[2] = 100;
    r[0] = 0;
    r[1] = 100;
    r[2] = 0;
    Collider_Init(q, 0, 0x5009, 0, 0x10, &a, &b);
    Collider_SetPosition(q, p + 0x40, 0);
    Collider_SetCallback(q, sub_082015D4, p);
    sub_0821FF7C(q, 0, *(u16 *)(p + 0x19a), *(u16 *)(p + 0x19c));
    Collider_Refresh(q);
    ((struct Q *)q)->flags |= 4;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08201784);
#endif
