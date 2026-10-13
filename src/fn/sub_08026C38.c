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
    register int shifted asm("r0");
    p->angle = angle;
    shifted = p->angle + 0x60;
    asm("" : "+r"(shifted));
    shifted >>= 6;
    value = 3 & shifted;
    p->facing = value;
    shifted = p->angle + 0x70;
    asm("" : "+r"(shifted));
    shifted >>= 5;
    value = 7 & shifted;
    p->facing8 = value;
}
void sub_0802175C(void *, void *, void *, int, int, int, int);
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
void sub_080216E8(void *, void *, void *, int, int, int);
void sub_0806C7B0(void *, void *, int, int, int, int, int, int, int, int, int, int, int);
typedef struct {
    s16 x, y, z;
} Vec;
static inline void add(Vec *a, Vec *b)
{
    a->x += b->x;
    a->y += b->y;
    a->z += b->z;
}
void sub_08026C38(ActorAnimation *p, int tick)
{
    ActorAnimation *p2 = p;
    Vec spawn;
    if (p->changed) {
        int sequence;
        p->changed = 0;
        setFacing(p, p->requestedAngle);
        sequence = choose(p->mode, 64, 26);
        {
            register int offset asm("r2") = 0x240;
            sub_080216E8(p, (u8 *)p + offset, (u8 *)p + 0x220, sequence, 2, 4);
        }
        sub_0822B3BC(859);
    }
    if (!p->tryUpdate(p)) {
        if (!p->reset(p)) {
            if (tick == 16)
                sub_0822B3BC(849);
            if (tick > 15 && (tick & 15) == 8) {
                u8 *anglePtr = &p->angle;
                s16 *velocity = (s16 *)&spawn;
                u32 angle = *anglePtr;
                {
                    int x = trunc12(gUnk_086149C4[(angle + 64) & 255] * 128);
                    register int zero asm("r0") = 0;
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
                        velocity[2] = z;
                    }
                }
                add(&spawn, (Vec *)((u8 *)p + 0x54));
                spawn.y += 300;
                sub_0806C7B0(&spawn, *(void **)((u8 *)p + 0x110), 2, p->requestedAngle, 90, 4, 20,
                             3, 25, *(s16 *)((u8 *)p2 + 0x37c), *(s16 *)((u8 *)p2 + 0x37e), 0x40000,
                             30);
            }
            if (*((u8 *)p2 + 0x25d) & 2)
                p->finished = 1;
        }
    }
}
