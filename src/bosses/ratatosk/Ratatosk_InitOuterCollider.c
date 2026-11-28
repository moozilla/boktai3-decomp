#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
#include "scene/collision/packed_contact_vector.h"
void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
void Collider_SetPosition(void *, void *, u32);
void Collider_SetCallback(void *, void *, void *);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void Ratatosk_OnOuterCollision(void);
void Ratatosk_InitOuterCollider(struct Ratatosk *p)
{
    struct ContactCollider *collider = &p->outerCollider;
    struct PackedContactVector size, offset;
    size.x = 130;
    size.y = 180;
    size.z = 130;
    offset.x = 0;
    offset.y = 180;
    offset.z = 0;
    Collider_Init(collider, 0, 0x2001, 0, 16, &size, &offset);
    Collider_SetPosition(collider, &p->anchor, 0);
    Collider_SetCallback(collider, Ratatosk_OnOuterCollision, p);
    Collider_SetAttackProperties(collider, p->bodyAttack[0], p->bodyAttack[1], 0, 0x40000,
                                 p->bodyAttack[2]);
}
