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

static inline void setFacing(ActorAnimation *p, int angle)
{
    int shifted;
    u32 value;
    p->angle = angle;
    shifted = p->angle + 0x60;
    shifted >>= 6;
    value = 3;
    value &= shifted;
    p->facing = value;
    shifted = p->angle + 0x70;
    shifted >>= 5;
    value = 7;
    value &= shifted;
    p->facing8 = value;
}
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
void Sound_PlaySongChecked(int);
int Math_Atan2(int, int);
extern s16 gUnk_086149C4[];
static inline int choose(u32 mode, int normal, int alternate)
{
    int test = mode & 1;
    int result;
    result = normal;
    if (test)
        result = alternate;
    return result;
}
static inline int trunc12(int x)
{
    return x >= 0 ? x >> 12 : -(-x >> 12);
}
static inline void decay(s16 *p)
{
    if (*p > 23)
        *p -= 24;
    else
        *p = 0;
}
void SceneActorGraphics_SelectResource(void *);
void SceneActors_UpdateShrinkingReaction(ActorAnimation *p)
{
    ActorAnimation *p2 = p;
    u32 zero;
    if (p->changed) {
        int sequence;
        int angle;
        p->changed = 0;
        angle = p->angle;
        setFacing(p, angle);
        sequence = choose(p->mode, 49, 11);
        {
            int offset = 0x240;
            SceneActors_StartDirectionalAnimation(p, (u8 *)p + offset, (u8 *)p + 0x220, sequence, 2,
                                                  1, 4);
        }
        SceneActorGraphics_SelectResource(p);
    }
    {
        u32 *dst = (u32 *)((u8 *)p + 0x33c);
        zero = 0;
        *dst = zero;
        *((u8 *)p + 0x340) = zero;
    }
    {
        u32 value;
        u32 mask = 12;
        value = p2->flags | mask;
        value |= 64;
        p2->flags = value;
    }
    if (p->minScale > 23)
        p->minScale -= 24;
    else
        p->minScale = zero;
    decay(&p->maxScale);
}
