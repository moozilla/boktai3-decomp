#include "global.h"
#include "scene/collision/packed_contact_halfwords.h"
#include "scene/collision/packed_contact_vector.h"
void Collider_Init(u8 *, u32, u32, u32, u32, struct PackedContactVector *,
                   struct PackedContactVector *);
void Collider_SetAttackProperties(u8 *, u32, u32, u32, u32, u32);
void Collider_SetCallback(u8 *, void (*)(void), u8 *);
void Collider_SetPosition(u8 *, u8 *, u32);
void Collider_Refresh(u8 *);
void SpritePulseEffect_OnAnimationEvent(void);
void SpritePulseEffect_InitCollider(u8 *p, u32 type, u32 a, u32 b, u32 c, u32 d)
{
    struct PackedContactVector s1, s2;
    u8 *q = p + 0x60;
    s1.x = 0x1e;
    s1.y = 0x1e;
    s1.z = 0x1e;
    s2.x = 0;
    s2.y = 0;
    s2.z = 0;
    Collider_Init(q, 0, 0x2101, 0, 1, &s1, &s2);
    Collider_SetAttackProperties(q, a, b, 0x10, c, d);
    Collider_SetCallback(q, SpritePulseEffect_OnAnimationEvent, p);
    Collider_SetPosition(q, p + 0x34, 0);
    p[0xa2] = type;
    Collider_Refresh(q);
}
