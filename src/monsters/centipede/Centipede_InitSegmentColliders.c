#include "monsters/centipede/centipede_body.h"
#include "scene/collision/contact_actor.h"
/* Centipede_InitSegmentColliders: Sets up body and attack colliders for every tentacle segment */

void Centipede_ReactToContact(void);
void Centipede_OnSegmentAttackCollision(void);

void Centipede_InitSegmentColliders(struct ChaseActor *actor)
{
    struct TentacleBody *body = actor->base.context;
    struct ContactPackedVector size, offset;
    s32 i;
    size.x = 64;
    size.y = 128;
    size.z = 64;
    offset.x = 0;
    offset.y = 128;
    offset.z = 0;
    for (i = 0; i < body->segmentCount; i++) {
        struct TentacleAnimationSegment *segment = &body->segments[i];
        struct ContactCollider *collider = &body->segments[i].bodyCollider;
        Collider_Init(collider, actor->base.body.id, 0x4109, 0, 16, &size, &offset);
        Collider_SetBodyProperties(collider, actor->base.field9E, 0, 0);
        Collider_SetPosition(collider, &body->segments[i].position, 0);
        Collider_SetCallback(collider, Centipede_ReactToContact, segment);
    }
    size.x = 64;
    size.y = 100;
    size.z = 64;
    offset.x = 0;
    offset.y = 128;
    offset.z = 0;
    for (i = 0; i < body->segmentCount; i++) {
        struct TentacleAnimationSegment *segment = &body->segments[i];
        struct ContactCollider *collider = &body->segments[i].attackCollider;
        Collider_Init(collider, actor->base.body.id, 0x2401, 0, 16, &size, &offset);
        {
            u16 first = actor->base.attacks.first[0];
            u16 second = actor->base.attacks.second[0];
            u32 flags = actor->base.attacks.flags[0];
            u16 parameter = actor->base.attacks.parameter[0];
            Collider_SetAttackProperties(collider, first, second, 0, flags, parameter);
        }
        Collider_SetPosition(collider, &body->segments[i].position, 0);
        Collider_SetCallback(collider, Centipede_OnSegmentAttackCollision, segment);
    }
}
