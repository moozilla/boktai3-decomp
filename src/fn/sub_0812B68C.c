#include "global.h"
#include "engine.h"

#include "pursuing_actor.h"
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void Collider_SetBodyProperties(void *, s32, s32, s32);
void SwooshProjectile_OnPrimaryColliderContact(void);
void SlashEffect_OnSecondaryColliderContact(void);
void PursuingActor_InitColliders(struct PursuingActor *actor, s32 a, s32 b, s32 c, s32 d)
{
    struct ActorColliderExtent size, offset;
    void *collider;
    void *position;
    void *secondary;
    size.x = 32;
    size.y = 64;
    size.z = 32;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    collider = &actor->primary;
    Collider_Init(collider, 0, 0x2401, 0, 16, &size, &offset);
    Collider_SetAttackProperties(collider, a, b, 0, c, d);
    Collider_SetCallback(collider, SwooshProjectile_OnPrimaryColliderContact, actor);
    position = &actor->position;
    Collider_SetPosition(collider, position, 0);
    secondary = &actor->secondary;
    Collider_Init(secondary, 0, 0x5000, 0, 16, &size, &offset);
    Collider_SetBodyProperties(secondary, 0, 0, 0);
    Collider_SetCallback(secondary, SlashEffect_OnSecondaryColliderContact, actor);
    Collider_SetPosition(secondary, position, 0);
    Collider_Refresh(secondary);
}
