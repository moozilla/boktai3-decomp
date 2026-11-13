#ifndef GUARD_ACTOR_RECOIL_H
#define GUARD_ACTOR_RECOIL_H
#include "scene_pool.h"
struct ActorRecoil {
    struct Task task;
    struct ScenePoolActor body;
    u8 collisionInfo[0x30];
    s8 group;
};
union ActorRecoilPoint {
    struct {
        s32 x : 16;
        s32 y : 16;
        s32 z : 16;
        s32 pad : 16;
    } packed;
    struct SceneVec value;
};
struct ActorRecoilVector {
    s32 x, y, z, w;
};
struct ActorRecoilVelocity {
    s16 x, y, z;
};
u8 ScenePool_CanAcceptControl(struct ScenePoolActor *);
void ActorRecoil_ReceiveBodyContact(struct ScenePoolActor *, struct ScenePoolContact *);
void ActorRecoil_ApplyDirectionalImpulse(struct ScenePoolActor *, struct ScenePoolContact *);
void ActorRecoil_InitCollisionVolumes(struct ActorRecoil *);
s32 ActorRecoil_UpdateMotion(struct ActorRecoil *);
void SceneMotion_PlayKindSound(struct ScenePoolContact *);
u16 ModifierStats_ApplyKind15(struct ScenePoolActor *, u16);
void SpatialIndex_InitEntry(void *, void *, void *, void *, s32, s32, s32, s32, s32, void (*)(),
                            void *);
void SpatialIndex_InsertEntry(void *);
s32 MotionTween_Advance(struct ScenePoolActor *, void *);
void ScenePool_ClampForwardBias(struct ScenePoolActor *);
u8 LinkParticipant_IsLocallySelected(struct ActorRecoil *);
void Character_UpdateLoopingSound(struct ScenePoolActor *);
#endif
