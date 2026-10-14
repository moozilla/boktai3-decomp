#include "contact_sequence_actor.h"
#define ContactSequenceActor_Initialize         sub_08045E08
#define ContactSequenceActor_ApplyHealthContact sub_08045D54
void ContactSequenceActor_ApplyHealthContact(void *, void *, struct ContactSequenceActor *);
void sub_0821FF7C(void *, u32, u32, u32);
void sub_0821FF58(void *, u32, u32, u32, u32, u32);
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
        sub_0821FF7C(collider, owner->contactValue, 0, 0);
        Collider_Refresh(collider);
        collider = &actor->resourceCollider;
        Collider_Init(collider, 0, 0x2001, 0, 16, &bounds, &offset);
        Collider_SetCallback(collider, 0, actor);
        sub_0821FF58(collider, 90, 50, 0, 0x60000, 30);
        actor->active = 0;
        actor->reactionValue = owner->reactionValue;
    }
}
