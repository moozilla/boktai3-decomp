#include "global.h"

/* Ratatosk field and state interpretations are provisional (low confidence). */
struct RatatoskActor {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[0x4a];
    u32 flags;
    u8 padA0[8];
    u8 facing, facing8;
    u8 padAA[4];
    u8 active, finished, padB0, changed, padB2, locked;
    u8 padB4[0x50];
    void *callback;
    u8 pad108[0xc];
    s8 savedAngle;
    u8 pad115[0x217];
    u8 animation[0x20];
    u8 animationData[0xb06];
    u16 motionE52, motionE54, padE56, motionE58;
    u8 padE5A[0xaa];
    s32 completionFrame;
};
void SceneActors_StartEightDirectionAnimation(void *, void *, void *, int, int, int, int);
s32 Callback_InvokeActor(void *, void *);
static inline void facing(struct RatatoskActor *p, int angle)
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
void Ratatosk_UpdateEightDirectionAnimation(struct RatatoskActor *p, s32 frame)
{
    struct RatatoskActor *q = p;
    if (p->changed) {
        p->changed = 0;
        if (p->locked) {
            int angle = p->savedAngle;
            facing(p, angle);
        }
        SceneActors_StartEightDirectionAnimation(p, p->animationData, p->animation, 2, 1, 1, 4);
    } else
        SceneActors_StartEightDirectionAnimation(p, p->animationData, p->animation, 2, 1, 1, 1);
    q->motionE52 = 960;
    q->motionE54 = 1440;
    q->motionE58 = 1;
    {
        u32 mask = 0x100000;
        p->flags = mask | p->flags;
    }
    if (!(u8)Callback_InvokeActor(p, p->callback)) {
        {
            u32 mask = 0x40;
            p->flags = mask | p->flags;
        }
        if (frame >= q->completionFrame && !p->locked)
            p->finished = 1;
    }
}
