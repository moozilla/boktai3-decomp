#include "global.h"
#include "scene/collision/packed_contact_vector.h"
void Collider_Init(u8 *, s32, s32, s32, s32, struct PackedContactVector *,
                   struct PackedContactVector *);
void Collider_SetAttackProperties(u8 *, s32, s32, s32, s32, s32);
void Collider_SetCallback(u8 *, void *, u8 *);
void Collider_SetPosition(u8 *, u8 *, s32);
void ChargedBurst_InitCollider(u8 *p, u32 id, s32 a, s32 b)
{
    struct PackedContactVector scale;
    struct PackedContactVector position;
    u8 *q = p + 0x20;
    scale.x = 100;
    scale.y = 100;
    scale.z = 100;
    position.x = 0;
    position.y = 0;
    position.z = 0;
    Collider_Init(q, 0, 0x2100, 0, (u16)id, &scale, &position);
    Collider_SetAttackProperties(q, a, 0, 0x20000, 0, 0);
    p[0x62] = b;
    Collider_SetCallback(q, 0, p);
    Collider_SetPosition(q, p + 0x8c, 0);
}
