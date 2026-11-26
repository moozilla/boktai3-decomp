#include "scene/actor/scene_actor_control_phase.h"
#include "global.h"
#include "scene/collision/scene_actor_collision.h"
struct Z {
    struct ContactPackedVector bits;
};
void SceneActorCollision_InitBody(struct ChaseActor *, void *, void *, void *, void *);
void SceneActorCollision_InitExternalAttackWithField34(struct ChaseActor *,
                                                       struct ContactCollider *, void *, void *,
                                                       void *, u32, u32, void *);
void SceneActorControl_ApplyContactPowerFloor(struct SceneActorBehaviorContactView *,
                                              struct SceneActorBehaviorContactView *,
                                              struct ChaseActor *);
void sub_08082648(struct SceneActorBehaviorContactView *, struct SceneActorBehaviorContactView *,
                  struct ChaseActor *);
void SceneActorControl_InitContactColliders(u8 *p)
{
    struct Z s1, s2, s3, s4;
    s1.bits.x = 0x20;
    s1.bits.y = 0x52;
    s1.bits.z = 0x20;
    s2.bits.x = 0;
    s2.bits.y = 0;
    s2.bits.z = 0;
    SceneActorCollision_InitBody((struct ChaseActor *)p, &s1, &s2,
                                 (void *)SceneActorControl_ApplyContactPowerFloor, p);
    s3.bits.x = 0x20;
    s3.bits.y = 0x2a;
    s3.bits.z = 0x20;
    s4.bits.x = 0;
    s4.bits.y = 0;
    s4.bits.z = 0;
    SceneActorCollision_InitExternalAttackWithField34((struct ChaseActor *)p,
                                                      (struct ContactCollider *)(p + 0x188), &s3,
                                                      &s4, (void *)sub_08082648, 0, 0x200000, p);
}
