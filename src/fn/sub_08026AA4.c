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
void SceneActors_StartAnimation(void *, void *, void *, int, int, int);
void RouteSpriteActor_Create(void *, void *, int, int, int, int, int, int, int, int, int, int);
typedef struct {
    s16 x, y, z;
} Vec;
static inline void add(Vec *a, Vec *b)
{
    a->x += b->x;
    a->y += b->y;
    a->z += b->z;
}
void SceneActors_UpdateRouteProjectileAttack(ActorAnimation *p, int tick)
{
    ActorAnimation *p2 = p;
    Vec spawn;
    if (p->changed) {
        int sequence;
        p->changed = 0;
        setFacing(p, p->requestedAngle);
        sequence = choose(p->mode, 64, 26);
        {
            int offset = 0x240;
            SceneActors_StartAnimation(p, (u8 *)p + offset, (u8 *)p + 0x220, sequence, 2, 4);
        }
        Sound_PlaySongChecked(890);
    }
    if (!p->tryUpdate(p)) {
        if (!p->reset(p)) {
            if (tick == 16)
                Sound_PlaySongChecked(849);
            if ((tick & 31) == 16) {
                u8 *anglePtr = &p->angle;
                s16 *velocity = (s16 *)&spawn;
                u32 angle = *anglePtr;
                u32 zeroCall;
                {
                    int x = trunc12(gUnk_086149C4[(angle + 64) & 255] * 128);
                    int zero = 0;
                    velocity[0] = x;
                    velocity[1] = zero;
                }
                {
                    int speed;
                    int sample = gUnk_086149C4[angle];
                    speed = 128;
                    asm("" : "+r"(speed) : "r"(sample));
                    {
                        int z = trunc12(sample * speed);
                        zeroCall = 0;
                        velocity[2] = z;
                    }
                }
                spawn.y = 256;
                add(&spawn, (Vec *)((u8 *)p + 0x54));
                RouteSpriteActor_Create(&spawn, *(void **)((u8 *)p + 0x110), 6, 6,
                                        *(s16 *)((u8 *)p2 + 0x378), *(s16 *)((u8 *)p2 + 0x37a),
                                        zeroCall, 20, 20, zeroCall, zeroCall, 3);
            }
            if (*((u8 *)p2 + 0x25d) & 2)
                p->finished = 1;
        }
    }
}
