#include "global.h"
#include "monsters/muspell_head.h"
#include "engine/engine.h"
void Collider_SetAttackProperties(void *, s32, s32, u32, u32, s32);
void Collider_SetBodyProperties(void *, u32, u32, u32);
void MuspellHead_OnPassiveColliderContact(void);
void MuspellHead_ApplyHitReaction(void);
// confidence=low: initializes passive and damage-response colliders embedded in the actor.
void MuspellHead_InitializeColliders(struct MuspellHead *head)
{
    struct ContactCollider *collider = &head->passiveCollider;
    struct ContactPosition bounds, offset;

    bounds.x = 120;
    bounds.y = 120;
    bounds.z = 120;
    offset.x = 0;
    offset.y = 120;
    offset.z = 0;
    Collider_Init(collider, 0, 0x2001, 0, 0x10, &bounds, &offset);
    Collider_SetCallback(collider, MuspellHead_OnPassiveColliderContact, head);
    Collider_SetAttackProperties(collider, head->attackA, head->attackB, 0, 0, head->attackC);

    collider = &head->bodyCollider;
    bounds.x = 100;
    bounds.y = 200;
    bounds.z = 100;
    offset.x = 0;
    offset.y = 100;
    offset.z = 0;
    Collider_Init(collider, 0, 0x5009, 0, 0x10, &bounds, &offset);
    Collider_SetPosition(collider, &head->sprite.position, 0);
    Collider_SetCallback(collider, MuspellHead_ApplyHitReaction, head);
    Collider_SetBodyProperties(collider, 0, head->damageType, head->damageFlags);
    Collider_Refresh(collider);
    collider->flags |= 4;
}
