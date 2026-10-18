#include "global.h"
#include "contact_actor.h"
#include "engine.h"
void CollisionReaction_RecordImpact(void);
void Collider_SetBodyProperties(void *, s32, s32, s32);
struct ImpactColliderActor {
    u8 unknown0[0x220];
    struct ContactCollider collider;
};
void ActorImpact_InitCollider(struct ImpactColliderActor *p)
{
    struct ContactPackedVector size, offset;
    struct ContactCollider *c = &p->collider;
    size.x = 256;
    size.y = 128;
    size.z = 256;
    *(u32 *)&offset = 0;
    ((u32 *)&offset)[1] &= 0xffff0000;
    Collider_Init(c, 0, 0x4002, 0, 16, &size, &offset);
    Collider_SetPosition(c, (u8 *)p + 0x2ec, 0);
    Collider_SetCallback(c, CollisionReaction_RecordImpact, p);
    Collider_SetBodyProperties(c, 0, 0x802, 1);
    Collider_Refresh(c);
}
