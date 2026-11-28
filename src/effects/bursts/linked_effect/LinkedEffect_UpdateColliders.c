#include "effects/bursts/linked_effect.h"

static inline void ClearFlags(struct ContactCollider *collider, u32 mask)
{
    collider->flags &= ~mask;
}

void LinkedEffect_UpdateColliders(struct LinkedEffect *effect)
{
    struct ContactCollider *attack, *body;
    struct ContactPosition *position;

    attack = &effect->colliders[0];
    position = &effect->sprite.position;
    Collider_SetPosition(attack, position, 0);
    body = &effect->colliders[1];
    Collider_SetPosition(body, position, 0);
    Collider_Refresh(attack);
    Collider_Refresh(body);
    if (body->responseValue) {
        body->responseValue--;
        body->flags |= 4;
    } else {
        ClearFlags(body, 4);
    }
}
