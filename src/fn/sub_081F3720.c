#include "global.h"
typedef struct SwoopActor {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[0x4a];
    u32 flags;
    u8 padA0[8];
    u8 facing, facing8;
    u8 padAA[5];
    u8 finished, padB0, changed, padB2, locked;
    u8 padB4[0x50];
    void *callback;
    u8 pad108[0xc];
    s8 savedAngle;
    u8 pad115[0x217];
    u8 animation[0x20];
    u8 animationData[0xb04];
    s16 distance;
    s16 motionE52, motionE54, motionE56, mode;
} SwoopActor;
void SceneActors_StartEightDirectionAnimation(void *, void *, void *, int, int, int, int);
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
s32 Callback_InvokeActor(void *, void *);
static inline void facing(SwoopActor *p, int angle)
{
    u32 v;
    int shifted;
    p->angle = angle;
    shifted = (p->angle + 0x60) >> 6;
    v = 3;
    v &= shifted;
    p->facing = v;
    shifted = (p->angle + 0x70) >> 5;
    v = 7;
    v &= shifted;
    p->facing8 = v;
}
void SceneActorAnimation_UpdateSwoopMotion(SwoopActor *p, s32 tick)
{
    SwoopActor *q = p;
    if (p->changed) {
        p->changed = 0;
        if (p->locked) {
            int angle = p->savedAngle;
            facing(p, angle);
        }
        if (q->mode == 0) {
            q->mode = 1;
            q->motionE56 = 0x20;
            SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 0, 2, 1, 4);
        } else {
            q->mode = 0;
            q->motionE56 = -32;
            SceneActors_StartEightDirectionAnimation(p, p->animationData, p->animation, 2, 1, 1, 4);
        }
    }
    if ((u8)Callback_InvokeActor(p, p->callback))
        return;
    if (q->mode == 0) {
        q->motionE52 = q->motionE52 * 3 >> 2;
        q->motionE54 = q->motionE54 * 0x3a >> 6;
    } else {
        q->motionE52 = (q->motionE52 * 0x3a + 0x1680) >> 6;
        q->motionE54 = (q->motionE54 * 0x30 + 0x5a00) >> 6;
    }
    if (q->mode == 1) {
        if (tick > 9)
            SceneActors_StartEightDirectionAnimation(p, q->animationData, q->animation, 2, 2, 1, 4);
        else
            SceneActors_StartDirectionalAnimation(p, q->animationData, q->animation, 0, 2, 1, 1);
    } else if (q->distance > 0x13f) {
        SceneActors_StartEightDirectionAnimation(p, q->animationData, q->animation, 2, 2, 1, 4);
    } else {
        SceneActors_StartDirectionalAnimation(p, q->animationData, q->animation, 0, 2, 1, 1);
    }
    {
        u32 mask = 0x40;
        p->flags = p->flags | mask;
    }
    if (tick > 0x2f) {
        if (q->mode == 0) {
            q->motionE52 = 0;
            q->motionE54 = 0;
        } else {
            q->motionE52 = 960;
            q->motionE54 = 1440;
        }
        p->finished = 1;
    }
}
