#include "repeated_effect.h"
struct H {
    u32 lo : 16;
    u32 hi : 16;
};
struct Q {
    u8 f[0x3e];
    u16 v;
};
struct Z {
    struct H a, b;
};
void Collider_Init(u8 *, u32, u32, u32, u32, struct Z *, struct Z *);
void Collider_SetCallback(u8 *,
                          void (*)(struct RepeatedEffectCollider *, struct RepeatedEffectCollider *,
                                   struct RepeatedEffect *),
                          struct RepeatedEffect *);
void Collider_SetPosition(u8 *, u8 *, u32);
s32 Collider_Refresh(u8 *);
void RepeatedEffect_HandleContact(struct RepeatedEffectCollider *, struct RepeatedEffectCollider *,
                                  struct RepeatedEffect *);
void RepeatedEffect_InitCollider(struct RepeatedEffect *p)
{
    struct Z s1, s2;
    u8 *q = (u8 *)&p->collider;
    s1.a.lo = 0x80;
    s1.a.hi = 0x32;
    s1.b.lo = 0x80;
    *(u32 *)&s2.a = 0;
    *(u32 *)&s2.b &= 0xFFFF0000;
    Collider_Init(q, p->tag, 0x4003, 0, 0x10, &s1, &s2);
    Collider_SetCallback(q, RepeatedEffect_HandleContact, p);
    Collider_SetPosition(q, (u8 *)&p->sprite.position, 0);
    Collider_Refresh(q);
    ((struct Q *)q)->v = 0x258;
}
