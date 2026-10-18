#define ActorCollision_IgnoreContact Axe_OnAttackColliderContact
#include "global.h"
#include "scene_actor_collision.h"
struct AuxiliaryAttackGroup {
    u8 unknown0[0x928];
    struct ContactCollider colliders[3];
};
void ActorCollision_IgnoreContact(void);
void ActorCollision_InitThreeAuxiliaryAttacks(struct SceneActorRegistryActor *actor)
{
    struct AuxiliaryAttackGroup *group = actor->context;
    struct ContactPackedVector size, offset;
    s32 i;
    size.x = 24;
    size.y = 64;
    size.z = 24;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    for (i = 0; i < 3; i++) {
        struct ContactCollider *collider = &group->colliders[i];
        Collider_Init(collider, actor->id, 0x2401, 0, 16, &size, &offset);
        Collider_SetAttackProperties(collider, SceneActorCollision_ReadAttackField3C(actor, 3),
                                     SceneActorCollision_ReadAttackField40(actor, 3), 0,
                                     SceneActorCollision_ReadAttackReactionFlags(actor, 3),
                                     SceneActorCollision_ReadAttackField44(actor, 3));
        Collider_SetPosition(collider, &actor->position, 0);
        Collider_SetCallback(collider, ActorCollision_IgnoreContact, actor);
    }
}
