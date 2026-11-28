#include "global.h"
#include "monsters/centipede/centipede_body.h"
#include "scene/collision/contact_actor.h"
void SceneActorCollision_InitBody(struct ChaseActor *, struct ContactPackedVector *,
                                  struct ContactPackedVector *, void *, struct ChaseActor *);
void SceneActorCollision_InitAttack(struct ChaseActor *, struct ContactPackedVector *,
                                    struct ContactPackedVector *, void *, u32, struct ChaseActor *);
void Centipede_InitCollision(struct ChaseActor *actor)
{
    struct ContactPackedVector bodySize, bodyOffset, attackSize, attackOffset;
    bodySize.x = 64;
    bodySize.y = 128;
    bodySize.z = 64;
    bodyOffset.x = 0;
    bodyOffset.y = 128;
    bodyOffset.z = 0;
    SceneActorCollision_InitBody(actor, &bodySize, &bodyOffset, (void *)CentipedeBody_OnBodyContact,
                                 actor);
    attackSize.x = 64;
    attackSize.y = 100;
    attackSize.z = 64;
    attackOffset.x = 0;
    attackOffset.y = 128;
    attackOffset.z = 0;
    SceneActorCollision_InitAttack(actor, &attackSize, &attackOffset,
                                   (void *)CentipedeBody_OnAttackCollision, 0, actor);
}
