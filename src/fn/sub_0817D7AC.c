#include "global.h"
#define SceneVolume_UpdateRepeatedShot sub_0817D7AC

struct VolumeActor {
    u8 pad0[0x51];
    u8 direction;
    u8 pad52[2];
    s16 position[4];
    s16 velocity[4];
    u8 pad64[0x38];
    u32 flags;
    u8 padA0[2];
    u8 phase;
    u8 padA3;
    u32 timer;
    u8 direction4, direction8;
    u8 state, padAB, action, subaction, choice, pending, queued, restart, finished, special;
    u8 padB4[0x10];
    u32 actionTimer;
    u8 padC8[4];
    u8 requestedDirection;
    u8 padCD[0x37];
    void *callback;
    u8 pad108[4];
    u8 *target;
    u8 pad110[4];
    s8 alternateDirection;
    u8 pad115[0x173];
    u8 bounds[0x1C];
    s16 destination[4];
    u8 animationState[0x20];
    u8 animation;
    u8 pad2CD[0x5F];
    u8 effect[0x13C];
    s16 height;
    u8 pad46A[6];
    u8 slowDown, impactDirection, speed, impactCount;
    u8 pad474[0x6C];
    u8 movementMode;
    u8 pad4E1;
    u8 reached, pathId, pointId;
    u8 pad4E5[3];
    u8 transition, previousChoice, decisionMode, decisionIndex;
    u8 pad4EC[8];
    s16 attackFlags;
    u8 pad4F6[6];
    s16 parameter;
    u8 pad4FE[0x722];
    u8 pathPosition[8];
};
extern const s16 gUnk_086149C4[];
void SceneActors_StartEightDirectionAnimation(void *, void *, void *, int, int, int, int);
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
void ProximityVolume_TranslateBounds(void *, void *);
void Sound_PlaySong(u32);
s32 ActorPosition_TestPath(void *, void *);
s32 SceneVolume_TestPlayerPosition(void *);
s32 SpriteComponent_GetCollisionStatus(void *);
void SceneParticleBurst_CreateWithPause(void *, void *, void *, int, int, int);
int Math_Atan2(int, int);
void ActorEffect_SpawnAtAngle(void *, void *, int);
u8 Callback_InvokeActor(void *, void *);
static inline void SetDirection(struct VolumeActor *p, int angle)
{
    p->direction = angle;
    {
        s32 d = (p->direction + 0x60) >> 6;
        u32 mask = 3;
        mask &= d;
        p->direction4 = mask;
    }
    {
        s32 d = (p->direction + 0x70) >> 5;
        u32 mask = 7;
        mask &= d;
        p->direction8 = mask;
    }
}
static inline s32 Scale(int x)
{
    if (x >= 0)
        return x >> 12;
    return -(-x >> 12);
}

s32 ActorMovement_CanReachTarget(void *);
s32 ActorImpact_TestForwardPoint(void *);
void ActorEffect_InitializeOffset(void *);
// confidence=low: alternates aim and projectile states, capped at three impacts
void SceneVolume_UpdateRepeatedShot(struct VolumeActor *p)
{
    struct VolumeActor *q = p;
    if (p->restart) {
        p->restart = 0;
        if (p->special)
            SetDirection(p, p->alternateDirection);
        else
            SetDirection(p, p->requestedDirection);
        SceneActors_StartEightDirectionAnimation(p, &q->animation, q->animationState, 0x134, 1, 1,
                                                 1);
        q->impactCount = 0;
        {
            u32 mask = 0x400004;
            p->flags |= mask;
        }
    }
    switch (p->phase) {
        case 0:
            if (p->timer == 0 && ActorMovement_CanReachTarget(q) == 1)
                SetDirection(p, p->requestedDirection);
            p->timer++;
            SceneActors_StartEightDirectionAnimation(p, &q->animation, q->animationState, 0x134, 1,
                                                     1, 1);
            if (p->finished) {
                u32 one = 1, zero = 0;
                p->phase = one;
                p->timer = zero;
            }
            break;
        case 1:
            SceneActors_StartEightDirectionAnimation(p, &q->animation, q->animationState, 0x139, 1,
                                                     1, 1);
            if (p->timer == 0)
                goto increment;
            if (p->timer > 10) {
                s32 result = ActorImpact_TestForwardPoint(p);
                if (result) {
                    u32 phase = 2, zero = 0;
                    p->phase = phase;
                    p->timer = zero;
                    ActorEffect_InitializeOffset(q);
                } else {
                    p->pending = 1;
                    p->phase = 3;
                    p->timer = result;
                }
            } else {
            increment:
                p->timer++;
            }
            break;
        case 2:
            if (++p->timer == 1)
                SceneActors_StartEightDirectionAnimation(p, &q->animation, q->animationState, 0x13E,
                                                         1, 1, 1);
            else if (p->timer > 1) {
                if (++q->impactCount > 3) {
                    p->pending = 1;
                    q->impactCount = 3;
                    p->phase = 3;
                    p->timer = 0;
                } else {
                    p->phase = 0;
                    p->timer = 0;
                }
            }
            break;
        case 3:
            if (p->timer == 0)
                SceneActors_StartEightDirectionAnimation(p, &q->animation, q->animationState, 0xF4,
                                                         1, 1, 1);
            p->timer++;
            break;
    }
    if (Callback_InvokeActor(p, p->callback))
        q->impactCount = 0;
    else {
        u32 mask = 0x40;
        p->flags |= mask;
    }
}
