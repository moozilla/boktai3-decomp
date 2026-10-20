#include "global.h"
// confidence=low: selects a reaction animation from the boss reaction mode.
#define Ratatosk_UpdateReactionAnimation sub_081F5910
struct Actor {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[0x56];
    u8 facing, facing8;
    u8 padAA[5];
    u8 finished, padB0, changed, padB2, locked;
    u8 padB4[0x60];
    s8 savedAngle;
    u8 pad115[0x217];
    u8 animation[0x20];
    u8 animationData[0xb48];
    s16 reaction;
};
void SceneActors_StartAnimation(void *, void *, void *, int, int, int);
static inline void facing(struct Actor *p, int angle)
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
void Ratatosk_UpdateReactionAnimation(struct Actor *p, s32 frame)
{
    struct Actor *q = p;
    if (p->changed) {
        p->changed = 0;
        if (p->locked)
            facing(p, p->savedAngle);
        if (p->reaction == 1)
            SceneActors_StartAnimation(p, p->animationData, p->animation, 0x21, 2, 4);
        else if (p->reaction == 0)
            SceneActors_StartAnimation(p, p->animationData, p->animation, 0x21, 1, 4);
        else
            SceneActors_StartAnimation(p, p->animationData, p->animation, 0x22, 2, 4);
    }
    if (frame == 16) {
        if (q->reaction == 1)
            p->finished = 1;
        else if (q->reaction == 0)
            SceneActors_StartAnimation(p, q->animationData, q->animation, 0x22, 1, 4);
        else
            goto other;
    }
    if (q->reaction == 0) {
        if (frame == 51)
            SceneActors_StartAnimation(p, q->animationData, q->animation, 0x23, 1, 4);
        if (frame > 56)
            p->finished = 1;
    } else {
    other:
        if (q->reaction == 1) {
            if (frame == 49)
                p->finished = 1;
        } else if (frame == 34)
            p->finished = 1;
    }
}
