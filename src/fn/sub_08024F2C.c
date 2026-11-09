#include "global.h"
typedef struct ActorAnimation {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[0x4a];
    u32 flags;
    u8 padA0[8];
    u8 facing, facing8;
    u8 padAA[5];
    u8 finished, enter;
    u8 changed;
    u8 padB2;
    u8 locked;
    u8 padB4[0x18];
    u8 requestedAngle;
    u8 padCD[0x37];
    u8 (*tryUpdate)(void *);
    u8 (*reset)(void *);
    u8 pad10C[8];
    s8 savedAngle;
    u8 pad115[0x22e];
    u8 variant;
    s16 scale, minScale, maxScale, scaleStep;
    u8 pad34C[0x60];
    u32 mode;
} ActorAnimation;

void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
void SceneActors_StartEightDirectionAnimation(void *, void *, void *, int, int, int, int);
static inline int choose(u32 mode, int normal, int alternate)
{
    int test = mode & 1;
    int result;
    result = normal;
    if (test)
        result = alternate;
    return result;
}
static inline int chooseMasked(u32 mode, u32 mask, int normal, int alternate)
{
    int test = mode & mask;
    int result;
    result = normal;
    if (test)
        result = alternate;
    return result;
}
static inline void startEight(ActorAnimation *p, ActorAnimation *a, u32 mode, int variant)
{
    int sequence = chooseMasked(mode, variant, 38, 0);
    SceneActors_StartEightDirectionAnimation(p, (u8 *)a + 0x240, (u8 *)a + 0x220, sequence, 2,
                                             variant, 4);
}
void SceneActorAnimation_TransitionVariant(ActorAnimation *p, int tick)
{
    ActorAnimation *p2 = p;
    int variant;
    if (p->changed) {
        p->changed = 0;
        if (!p->variant) {
            int sequence;
            p->variant = 1;
            p->scaleStep = 32;
            sequence = choose(p->mode, 45, 7);
            {
                int offset = 0x240;
                SceneActors_StartDirectionalAnimation(p, (u8 *)p + offset, (u8 *)p + 0x220,
                                                      sequence, 2, 1, 4);
            }
        } else {
            int sequence;
            p->variant = 0;
            p->scaleStep = -32;
            sequence = choose(p->mode, 47, 9);
            {
                int offset = 0x240;
                SceneActors_StartDirectionalAnimation(p, (u8 *)p + offset, (u8 *)p + 0x220,
                                                      sequence, 2, 1, 4);
            }
        }
    }
    if (p->tryUpdate(p))
        return;
    {
        u32 mask = 0x40;
        p->flags |= mask;
    }
    if (p->reset(p))
        return;
    if (!p2->variant) {
        p2->minScale = (p2->minScale * 3) >> 2;
        p2->maxScale = (p2->maxScale * 58) >> 6;
    } else {
        p2->minScale = (p2->minScale * 58 + 5760) >> 6;
        p2->maxScale = (p2->maxScale * 48 + 23040) >> 6;
    }
    variant = p2->variant;
    if (variant == 1) {
        int sequence;
        if (tick > 9) {
            startEight(p, p2, p2->mode, variant);
        } else {
            if (p2->scale > 319)
                sequence = chooseMasked(p2->mode, variant, 47, 9);
            else
                sequence = chooseMasked(p2->mode, variant, 45, 7);
            SceneActors_StartDirectionalAnimation(p, (u8 *)p2 + 0x240, (u8 *)p2 + 0x220, sequence,
                                                  2, 1, 1);
        }
    } else if (p2->scale > 319) {
        int sequence;
        startEight(p, p2, p2->mode, 1);
    } else {
        int sequence;
        if (p2->scale > 79)
            sequence = choose(p2->mode, 47, 9);
        else
            sequence = choose(p2->mode, 45, 7);
        SceneActors_StartDirectionalAnimation(p, (u8 *)p2 + 0x240, (u8 *)p2 + 0x220, sequence, 2, 1,
                                              1);
    }
    if (tick > 47) {
        if (!p2->variant) {
            p2->minScale = 0;
            p2->maxScale = 0;
        } else {
            p2->minScale = 960;
            p2->maxScale = 1440;
        }
        p->finished = 1;
    }
}
