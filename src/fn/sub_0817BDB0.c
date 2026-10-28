#include "global.h"
#include "engine_globals.h"

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

extern u16 gUnk_0203B400[];
s32 Mod(s32, s32);
void Path_GetPointPositionById(void *, int, int);
s32 SceneVolume_GetActivityState(void *);
void SceneVolume_BeginPhase(void *, int);
s32 SceneVolume_ConsumePendingEvent(void *);
void AnimatedHazard_CreateProximity(void *, int, int, int);
static inline u8 TakeQueued(struct VolumeActor *p)
{
    if (p->queued) {
        p->queued = 0;
        p->pending = 0;
        return 1;
    }
    return 0;
}
static inline s32 RandomValue(void)
{
    u16 *values = gUnk_0203B400;
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(u16 *)(offset + (u32)values);
}
// confidence=low: follows path points while creating proximity hazards
void ActorDecision_UpdateHazardPath(struct VolumeActor *p)
{
    struct VolumeActor *q = p;
    if (TakeQueued(p)) {
        q->movementMode = 3;
        q->pathId = (RandomValue() >> 8) & 3;
        q->pointId = Mod(RandomValue(), 5);
        Path_GetPointPositionById(q->pathPosition, q->pathId, q->pointId);
        {
            u32 state = 4;
            p->state = state;
        }
        {
            u32 mask = 2;
            q->attackFlags |= mask;
        }
    }
    if (SceneVolume_GetActivityState(p))
        SceneVolume_BeginPhase(p, 2);
    if (p->pending == 1) {
        if (SceneVolume_ConsumePendingEvent(q)) {
            q->pointId++;
            if (++p->actionTimer > 3) {
                p->previousChoice = p->choice;
                p->transition = 1;
                p->decisionIndex = 0;
            } else
                AnimatedHazard_CreateProximity(p->position, q->parameter, 800, 60);
            Path_GetPointPositionById(q->pathPosition, q->pathId, q->pointId);
            q->movementMode = 3;
            {
                u32 state = 4;
                p->state = state;
            }
        } else {
            q->movementMode = 3;
            {
                u32 state = 4;
                p->state = state;
            }
        }
    }
}
