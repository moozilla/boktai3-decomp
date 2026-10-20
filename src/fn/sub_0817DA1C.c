#include "global.h"

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

// confidence=low: pursues a destination while spawning a rotating effect
void SceneVolume_UpdatePursuitSpin(struct VolumeActor *p, s32 elapsed)
{
    struct VolumeActor *q = p;
    if (p->restart) {
        p->restart = 0;
        SetDirection(p, p->requestedDirection);
        SceneActors_StartEightDirectionAnimation(p, &p->animation, p->animationState, 0x134, 1, 1,
                                                 4);
        if (p->movementMode == 3)
            ProximityVolume_TranslateBounds(p->bounds, p->position);
        else if (p->movementMode == 1)
            ProximityVolume_TranslateBounds(p->bounds, p->position);
        {
            u32 mask = 0x400004;
            p->flags |= mask;
        }
        q->impactCount = 0;
        Sound_PlaySong(0x2E9);
    }
    if (q->movementMode == 3) {
        int result = ActorPosition_TestPath(q, q->pathPosition);
        if (!result)
            q->movementMode = result;
    } else if (q->movementMode == 1) {
        int result = SceneVolume_TestPlayerPosition(q);
        if (!result)
            q->movementMode = result;
    }
    switch (p->phase) {
        case 0: {
            u32 mask = 0x400000;
            p->flags |= mask;
        }
            if (p->timer == 0)
                SceneActors_StartEightDirectionAnimation(p, &q->animation, q->animationState, 0x134,
                                                         1, 1, 4);
            if (++p->timer > 20) {
                p->phase = 1;
                p->timer = 0;
            }
            break;
        case 1: {
            u32 mask = 0x100000;
            p->flags |= mask;
        }
            if (SpriteComponent_GetCollisionStatus(q->effect) && !q->impactCount) {
                q->impactCount++;
                Sound_PlaySong(0x2E7);
            }
            if (!(elapsed & 3)) {
                struct {
                    s16 x, y, z, pad;
                } a, b;
                a.x = 0;
                a.y = 0;
                a.z = 0;
                b.x = 0x20;
                b.y = 0;
                b.z = 0x20;
                SceneParticleBurst_CreateWithPause(p->position, &a, &b, 1, 2, 4);
            }
            if (q->movementMode == 1 || q->movementMode == 3) {
                int speed = 5;
                int dx = q->destination[0] - p->position[0];
                int dz = q->destination[2] - p->position[2];
                if (dx * dx + dz * dz <= 25) {
                    p->velocity[0] = dx;
                    p->velocity[2] = dz;
                    q->reached = 1;
                } else {
                    int angle = Math_Atan2(dx, dz) & 0xE0;
                    p->velocity[0] = Scale(gUnk_086149C4[(angle + 0x40) & 255] * speed);
                    p->velocity[2] = Scale(gUnk_086149C4[angle] * speed);
                }
            }
            p->timer = (p->timer + 16) & 255;
            ActorEffect_SpawnAtAngle(q->effect, p->position, p->timer);
            SetDirection(p, p->timer + 15);
            SceneActors_StartEightDirectionAnimation(p, &q->animation, q->animationState, 0x13E, 1,
                                                     1, 1);
            if (elapsed > 96) {
                p->phase = 2;
                p->timer = 0;
            }
            if (elapsed <= 31) {
                u32 mask = 0x400000;
                p->flags |= mask;
            }
            break;
        case 2:
            q->impactCount = 0;
            p->pending = 1;
            SetDirection(p, p->requestedDirection);
            SceneActors_StartEightDirectionAnimation(p, &q->animation, q->animationState, 0x134, 1,
                                                     1, 4);
            break;
    }
    {
        u32 mask = 0x40;
        p->flags |= mask;
    }
    Callback_InvokeActor(p, p->callback);
}
