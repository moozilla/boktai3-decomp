#include "global.h"
#include "engine.h"
#include "hresvelgr_travel.h"

void Hresvelgr_OnBodyContact(void);
void Collider_SetBodyProperties(void *, s32, s32, s32);

static inline void SetVector(struct SceneVec *v, s32 x, s32 y, s32 z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}

void ColliderActor_InitBodyCollider(struct HresvelgrTravelActor *actor, s32 mode)
{
    void *collider = actor->bodyCollider;
    struct SceneVec size, offset;
    size.x = 256;
    size.y = 256;
    size.z = 256;
    offset.x = 0;
    offset.y = 320;
    offset.z = 0;
    actor->bodyOffset = offset;
    SetVector(&actor->bodyRotation, 0, 64, 0);
    Collider_Init(collider, 0, 0x5101, 0, 16, &size, &offset);
    Collider_SetPosition(collider, &actor->position, 0);
    Collider_SetCallback(collider, Hresvelgr_OnBodyContact, actor);
    Collider_SetBodyProperties(collider, mode, 0x810, 32);
    Collider_Refresh(collider);
}
