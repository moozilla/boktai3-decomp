#include "global.h"
#include "scene_actor_collision.h"
void Dog_OnBodyCollision(void);
void Dog_OnSensorContact(void);
void Dog_OnAttackContact(void);
void SceneActorCollision_InitBody(struct SceneActorRegistryActor *, void *, void *, void *, void *);
void SceneActorCollision_InitAttack(struct SceneActorRegistryActor *, void *, void *, void *, u32,
                                    void *);
void SceneActorCollision_InitExternalAttack(struct SceneActorRegistryActor *,
                                            struct ContactCollider *, void *, void *, void *, u32,
                                            void *);
void SceneActorCollision_InitGateColliders(struct SceneActorRegistryActor *actor)
{
    struct ContactPackedVector bodySize, bodyOffset, attackSize, attackOffset, externalSize,
        externalOffset;
    u8 *context = actor->context;
    bodySize.x = 0x40;
    bodySize.y = 0x80;
    bodySize.z = 0x40;
    bodyOffset.x = 0;
    bodyOffset.y = 0x80;
    bodyOffset.z = 0;
    SceneActorCollision_InitBody(actor, &bodySize, &bodyOffset, Dog_OnBodyCollision, actor);
    attackSize.x = 0x40;
    attackSize.y = 0x78;
    attackSize.z = 0x40;
    attackOffset.x = 0;
    attackOffset.y = 0x80;
    attackOffset.z = 0;
    SceneActorCollision_InitAttack(actor, &attackSize, &attackOffset, Dog_OnSensorContact, 0,
                                   actor);
    externalSize.x = 0;
    externalSize.y = 0x78;
    externalSize.z = 0;
    externalOffset.x = 0;
    externalOffset.y = 0x80;
    externalOffset.z = 0;
    SceneActorCollision_InitExternalAttack(actor, (struct ContactCollider *)(context + 0x624),
                                           &externalSize, &externalOffset, Dog_OnAttackContact, 1,
                                           actor);
    {
        u32 mask = 0x100;
        struct ContactCollider *collider = (struct ContactCollider *)(context + 0x624);
        mask |= collider->flags;
        collider->flags = mask;
    }
}
