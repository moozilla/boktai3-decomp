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
    u32 value;
    int shifted;
    p->angle = angle;
    shifted = p->angle + 0x60;
    shifted >>= 6;
    value = 3 & shifted;
    p->facing = value;
    shifted = p->angle + 0x70;
    shifted >>= 5;
    value = 7 & shifted;
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
void SceneActors_UpdateKnockbackReaction(ActorAnimation *base, int time)
{
    int tick = time;
    ActorAnimation *p = base;
    u8 *position = (u8 *)p + 0x48;
    u32 zero;
    if (p->changed) {
        int sequence;
        p->changed = 0;
        sequence = choose(p->mode, 49, 11);
        {
            int offset = 0x240;
            SceneActors_StartDirectionalAnimation(p, (u8 *)p + offset, (u8 *)p + 0x220, sequence, 2,
                                                  1, 4);
        }
    }
    {
        u32 *dst = (u32 *)((u8 *)p + 0x33c);
        zero = 0;
        *dst = zero;
        *((u8 *)p + 0x340) = zero;
    }
    {
        u32 mask = 12;
        base->flags |= mask;
    }
    if (p->minScale > 23)
        p->minScale -= 24;
    else
        p->minScale = zero;
    decay(&p->maxScale);
    if (!base->reset(base)) {
        if (tick <= 9) {
            u8 *anglePtr = (u8 *)p + 0x342;
            s16 *velocity = (s16 *)(position + 20);
            u32 angle;
            int speed;
            {
                u32 value = *anglePtr;
                angle = value;
            }
            speed = 24;
            {
                int x = trunc12(gUnk_086149C4[(angle + 64) & 255] * speed);
                int z = 0;
                velocity[0] = x;
                velocity[1] = z;
            }
            velocity[2] = trunc12(gUnk_086149C4[angle] * speed);
        }
        if (tick > 19) {
            u32 z;
            u8 *dst = &base->finished;
            z = 0;
            *dst = 1;
            p->variant = z;
        }
    }
}
