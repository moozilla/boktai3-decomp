#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
#include "scene/collision/packed_contact_vector.h"
void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
void Collider_SetPosition(void *, void *, u32);
void Collider_SetCallback(void *, void *, void *);
void Collider_SetBodyProperties(void *, s32, s32, s32);
s32 Collider_Refresh(void *);
void PreviewActor_ReplaceProjectileTowardPlayer(void);
void Ratatosk_InitWideBodyCollider(struct Ratatosk *p)
{
    struct PackedContactVector size, offset;
    struct ContactCollider *collider = &p->wideBodyCollider;
    size.x = 140;
    size.y = 180;
    size.z = 140;
    offset.x = 0;
    offset.y = 180;
    offset.z = 0;
    Collider_Init(collider, 0, 0x4005, 0, 16, &size, &offset);
    Collider_SetPosition(collider, &p->anchor, 0);
    Collider_SetCallback(collider, PreviewActor_ReplaceProjectileTowardPlayer, p);
    Collider_SetBodyProperties(collider, 0, 0x804, 8);
    Collider_Refresh(collider);
}
