#include "effects/bursts/linked_effect.h"
#include "scene/collision/packed_contact_vector.h"

void Collider_SetAttackProperties(struct ContactCollider *, s32 power, s32 knockbackPower,
                                  s32 weakness, u32 attributes, s32 invincibilityFrames);
void Collider_SetBodyProperties(void *, u16, u32, u32);
void ActorEffect_OrientFromPositions(void);
void LinkedEffect_ApplyVariant(void);

void ActorEffect_InitColliders(struct LinkedEffect *effect, s32 attackPower, s32 knockbackPower,
                               u32 attributes, s32 invincibilityFrames)
{
    struct PackedContactVector size, offset;
    struct ContactCollider *attack, *body;
    struct ContactPosition *position;

    size.x = 64;
    size.y = 128;
    size.z = 64;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    attack = &effect->colliders[0];
    Collider_Init(attack, 0, 0x2001, 0, 16, &size, &offset);
    Collider_SetAttackProperties(attack, attackPower, knockbackPower, 0, attributes,
                                 invincibilityFrames);
    Collider_SetCallback(attack, ActorEffect_OrientFromPositions, effect);
    position = &effect->sprite.position;
    Collider_SetPosition(attack, position, 0);
    size.x = 92;
    size.y = 128;
    size.z = 92;
    body = &effect->colliders[1];
    Collider_Init(body, 0, 0x4005, 0, 16, &size, &offset);
    Collider_SetBodyProperties(body, 0, 0, 0);
    Collider_SetCallback(body, LinkedEffect_ApplyVariant, effect);
    Collider_SetPosition(body, position, 0);
    effect->colliders[1].responseValue = 60;
}
