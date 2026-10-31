#include "global.h"
#include "scene_actor_collision.h"
struct AxeColliderBlock {
    u8 unknown0[0xCEC];
    struct ContactCollider attack[2];
    struct ContactCollider body[2];
};
void SceneActorCollision_InitBody(struct SceneActorRegistryActor *, void *, void *, void *, void *);
void Axe_OnBodyContact(void);
void Axe_OnAttackColliderContact(void);
void Axe_InitColliders(struct SceneActorRegistryActor *actor)
{
    struct AxeColliderBlock *blk = actor->context;
    struct ContactPackedVector bodySize, bodyOff, atkSize, atkOff;
    s32 i;
    u16 f = 0x2401;
    u32 k;
    bodySize.x = 0x18;
    bodySize.y = 0x46;
    bodySize.z = 0x18;
    bodyOff.x = 0;
    bodyOff.y = 0;
    bodyOff.z = 0;
    SceneActorCollision_InitBody(actor, &bodySize, &bodyOff, Axe_OnBodyContact, actor);
    for (i = 0; i < 2; i++) {
        struct ContactCollider *c = &blk->body[i];
        Collider_Init(c, actor->id, 0x4001, 0, 16, &bodySize, &bodyOff);
        Collider_SetBodyProperties(c, SceneActorCollision_ReadBodyField3C(actor),
                                   SceneActorCollision_ReadBodyReactionFlags(actor),
                                   SceneActorCollision_ReadBodyField34(actor));
        Collider_SetPosition(c, &actor->position, 0);
        Collider_SetCallback(c, Axe_OnBodyContact, actor);
        Collider_Refresh(c);
    }
    atkSize.x = 0x18;
    atkSize.y = 0x40;
    atkSize.z = 0x18;
    atkOff.x = 0;
    atkOff.y = 0;
    atkOff.z = 0;
    {
        struct ContactCollider *c = (struct ContactCollider *)((u8 *)actor + 0x188);
        Collider_Init(c, actor->id, f, 0, 16, &atkSize, &atkOff);
        Collider_SetAttackProperties(c, SceneActorCollision_ReadAttackField3C(actor, 0),
                                     SceneActorCollision_ReadAttackField40(actor, 0), 0,
                                     SceneActorCollision_ReadAttackReactionFlags(actor, 0),
                                     SceneActorCollision_ReadAttackField44(actor, 0));
        Collider_SetPosition(c, &actor->position, 0);
        Collider_SetCallback(c, Axe_OnAttackColliderContact, actor);
    }
    k = 0x24010000;
    for (i = 0; i < 2; i++) {
        struct ContactCollider *c = &blk->attack[i];
        Collider_Init(c, actor->id, k >> 16, 0, 16, &atkSize, &atkOff);
        Collider_SetAttackProperties(c, SceneActorCollision_ReadAttackField3C(actor, 0),
                                     SceneActorCollision_ReadAttackField40(actor, 0), 0,
                                     SceneActorCollision_ReadAttackReactionFlags(actor, 0),
                                     SceneActorCollision_ReadAttackField44(actor, 0));
        Collider_SetPosition(c, &actor->position, 0);
        Collider_SetCallback(c, Axe_OnAttackColliderContact, actor);
    }
}
