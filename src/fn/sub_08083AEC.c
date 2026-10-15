#include "scene_actor_control_phase.h"
#include "global.h"
struct Z { struct ContactPackedVector bits; };
void SceneActorCollision_InitBody(u8 *, struct Z *, struct Z *, void (*)(void), u8 *);
void SceneActorCollision_InitExternalAttackWithField34(u8 *, u8 *, struct Z *, struct Z *,
                                                       void (*)(void), u32, u32, u8 *);
void SceneActorControl_ApplyContactPowerFloor(struct SceneActorBehaviorContactView *,
                                              struct SceneActorBehaviorContactView *,
                                              struct SceneActorRegistryResetView *);
void sub_08082648(void);
void sub_08083AEC(u8 *p)
{
    struct Z s1, s2, s3, s4;
    s1.bits.x = 0x20;
    s1.bits.y = 0x52;
    s1.bits.z = 0x20;
    s2.bits.x = 0;
    s2.bits.y = 0;
    s2.bits.z = 0;
    SceneActorCollision_InitBody(p, &s1, &s2,
                                 (void (*)(void))SceneActorControl_ApplyContactPowerFloor, p);
    s3.bits.x = 0x20;
    s3.bits.y = 0x2a;
    s3.bits.z = 0x20;
    s4.bits.x = 0;
    s4.bits.y = 0;
    s4.bits.z = 0;
    SceneActorCollision_InitExternalAttackWithField34(p, p + 0x188, &s3, &s4, sub_08082648, 0,
                                                      0x200000, p);
}
