#include "global.h"
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
void Collider_SetAttackProperties(u8 *, s32, s32, u32, u32, s32);
void Collider_SetPosition(u8 *, u8 *, u32);
void Collider_SetBodyProperties(void *, u32, u32, u32);
s32 Collider_Refresh(u8 *);
void ScriptCombatActor_OnPassiveColliderContact(void);
void ScriptCombatActor_ApplyHitReaction(void);
// confidence=low: initializes passive and damage-response colliders embedded in the actor.
#define ScriptCombatActor_InitializeColliders sub_08201784
void ScriptCombatActor_InitializeColliders(u8 *pArg)
{
    u8 *p = pArg;
    struct Z a, b;
    u8 *q = p + 0x6c;
    u16 *v, *r;
    a.a.lo = 120;
    a.a.hi = 120;
    a.b.lo = 120;
    *(u32 *)&b.a = 0x780000;
    *(u32 *)&b.b &= 0xffff0000;
    Collider_Init(q, 0, 0x2001, 0, 0x10, &a, &b);
    Collider_SetCallback(q, ScriptCombatActor_OnPassiveColliderContact, p);
    Collider_SetAttackProperties(q, *(s16 *)(p + 0x174), *(s16 *)(p + 0x176), 0, 0,
                                 *(s16 *)(p + 0x178));
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
    Collider_SetCallback(q, ScriptCombatActor_ApplyHitReaction, p);
    Collider_SetBodyProperties(q, 0, *(u16 *)(p + 0x19a), *(u16 *)(p + 0x19c));
    Collider_Refresh(q);
    ((struct Q *)q)->flags |= 4;
}
