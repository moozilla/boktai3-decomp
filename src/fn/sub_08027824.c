// MATCHING_CONCESSION: register-pinning
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
    register u32 value asm("r1");
    register int shifted asm("r0");
    p->angle = angle;
    shifted = p->angle + 0x60;
    asm("" : "+r"(shifted));
    shifted >>= 6;
    value = 3 & shifted;
    asm("" : "+r"(value));
    p->facing = value;
    shifted = p->angle + 0x70;
    asm("" : "+r"(shifted));
    shifted >>= 5;
    value = 7 & shifted;
    asm("" : "+r"(value));
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
static inline void effect(u8 *p, int id, int mode, int duration)
{
    *(u32 *)(p + 0x390) = id;
    p[0x38e] = mode;
    p[0x38f] = duration;
    if (p[0x38e] == 2) {
        u32 value = duration == 5 ? 32 : 64;
        p[0x38c] = value;
    } else if (p[0x38e] == 1)
        p[0x38c] = 0;
}
void SceneActors_UpdateDirectionalTurn(ActorAnimation *p)
{
    ActorAnimation *p2 = p;
    u8 *position = (u8 *)p + 0x48;
    if (p->changed) {
        int sequence;
        p->changed = 0;
        sequence = choose(p->mode, 43, 5);
        {
            int offset = 0x240;
            SceneActors_StartDirectionalAnimation(p, (u8 *)p + offset, (u8 *)p + 0x220, sequence, 1,
                                                  1, 4);
        }
    } else {
        int sequence = choose(p->mode, 43, 5);
        {
            int offset = 0x240;
            SceneActors_StartDirectionalAnimation(p, (u8 *)p + offset, (u8 *)p + 0x220, sequence, 1,
                                                  1, 1);
        }
    }
    *(u32 *)((u8 *)p + 0x33c) = 0;
    *((u8 *)p + 0x340) = 0;
    {
        u32 mask = 12;
        p2->flags |= mask;
    }
    if (!p2->reset(p2)) {
        int delta = (s8)(position[9] - (u8)p2->savedAngle), distance = delta;
        if (distance < 0)
            distance = -distance;
        if (distance <= 7) {
            setFacing(p2, p2->savedAngle);
            p2->finished = 1;
        } else {
            int angle;
            if (delta < 0)
                angle = position[9] + 8;
            else
                angle = position[9] - 8;
            setFacing(p2, angle);
        }
        p->minScale = 0;
        p->maxScale = 0;
        p->variant = 0;
    }
}
