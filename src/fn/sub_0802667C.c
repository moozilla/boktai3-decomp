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
void sub_0822B3BC(int);
int sub_082215E4(int, int);
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
    } else if (p[0x38e] == 1) {
        u8 *dst = p + 0x38c;
        u32 zero;
        zero = 0;
        asm("" : "+r"(zero));
        *dst = zero;
    }
}
void SceneActors_UpdateAnimation(void *, void *, void *);
void sub_08022AF4(void *);
void sub_0802667C(ActorAnimation *p, int tick)
{
    ActorAnimation *p2 = p;
    if (p->changed) {
        int sequence;
        u8 *draw, *resource;
        p->changed = 0;
        sequence = choose(p->mode, 66, 28);
        draw = (u8 *)p + 0x240;
        resource = (u8 *)p + 0x220;
        SceneActors_StartDirectionalAnimation(p, draw, resource, sequence, 2, 1, 0);
        *(u16 *)((u8 *)p + 0x254) = 2;
        SceneActors_UpdateAnimation(p, draw, resource);
        effect((u8 *)p, 826, 2, 5);
        sub_08022AF4(p2);
    }
    if (!p->tryUpdate(p)) {
        u32 mask = 64;
        p->flags |= mask;
        if (!p->reset(p) && tick > 30)
            p->finished = 1;
    }
}
