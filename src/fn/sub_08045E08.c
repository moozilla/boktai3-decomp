#include "contact_sequence_actor.h"
void ContactSequenceActor_ApplyHealthContact(void *, void *, struct ContactSequenceActor *);
void Collider_SetBodyProperties(void *, u32, u32, u32);
void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);
void ContactSequenceActor_Initialize(struct ContactSequenceOwner *owner,
                                     struct ContactSequenceActor *actor)
{
    struct ContactSwarmGraphics *graphics = &actor->graphics;
    struct ContactSwarmSprite *sprite;
    struct ContactPackedVector bounds, offset;
    struct ContactCollider *collider;
    if (AnimData_Load(graphics, 0x4147)) {
        sprite = &actor->sprite;
        Sprite_Init(sprite, graphics, 1);
        sprite->priority = 2;
        Anim_SetSequence(&actor->sequence, owner->sequenceResource, 0, 0, 0);
        bounds.x = 80;
        bounds.y = 128;
        bounds.z = 80;
        offset.x = 0;
        offset.y = 128;
        offset.z = 0;
        collider = &actor->healthCollider;
        Collider_Init(collider, 0, 0x4005, 0, 16, &bounds, &offset);
        Collider_SetCallback(collider, ContactSequenceActor_ApplyHealthContact, actor);
        Collider_SetBodyProperties(collider, owner->contactValue, 0, 0);
        Collider_Refresh(collider);
        collider = &actor->resourceCollider;
        Collider_Init(collider, 0, 0x2001, 0, 16, &bounds, &offset);
        Collider_SetCallback(collider, 0, actor);
        Collider_SetAttackProperties(collider, 90, 50, 0, 0x60000, 30);
        actor->active = 0;
        actor->reactionValue = owner->reactionValue;
    }
}
