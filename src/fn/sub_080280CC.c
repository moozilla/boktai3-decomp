// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "ram.h"
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
extern u16 gUnk_0203B400[];
void ContactSwarm_SpawnCharge(void *, void *, int, int, int, int, int);
static inline void vector(s16 *dst, int direction, int length)
{
    s16 *velocity;
    register u32 angle asm("r4");
    int speed;
    {
        register u32 converted asm("r0") = direction << 24;
        velocity = dst;
        angle = converted >> 24;
    }
    speed = length;
    {
        int x = trunc12(gUnk_086149C4[(angle + 64) & 255] * speed);
        int zero = 0;
        velocity[0] = x;
        velocity[1] = zero;
    }
    {
        register s16 *entry asm("r0") = &gUnk_086149C4[angle];
        velocity[2] = trunc12(*entry * speed);
    }
}
typedef struct {
    s16 x, y, z;
} Vec;
static inline int add(Vec *a, Vec *b)
{
    int zero;
    a->x += b->x;
    zero = 0;
    a->y += b->y;
    a->z += b->z;
    return zero;
}
static inline int turn(int target, int current)
{
    int angle = current;
    int delta = target - angle;
    if (delta > 32)
        delta = 32;
    else {
        int lower = -32;
        if (delta < lower)
            delta = lower;
    }
    return angle + delta;
}
void SceneActors_UpdateSwarmAttack(ActorAnimation *p, int tick)
{
    ActorAnimation *p2 = p;
    u8 *position = (u8 *)p + 0x48;
    Vec spawn;
    if (p->changed) {
        int sequence;
        p->changed = 0;
        sequence = choose(p->mode, 68, 30);
        {
            int offset = 0x240;
            SceneActors_StartDirectionalAnimation(p, (u8 *)p + offset, (u8 *)p + 0x220, sequence, 2,
                                                  1, 4);
        }
        Sound_PlaySongChecked(860);
    }
    if (p->tryUpdate(p))
        return;
    if (p->reset(p))
        return;
    if (tick > 10 && (tick & 7) == 7) {
        int zero;
        int direction;
        if (p->facing == 0 || p->facing == 3)
            vector((s16 *)&spawn, (p->facing * 64) - 64, 180);
        else
            vector((s16 *)&spawn, (p->facing * 64) - 64, 110);
        zero = add(&spawn, (Vec *)(position + 12));
        spawn.y += 300;
        direction = *((u8 *)p2 + 0x3ba);
        gRandomIndex = (gRandomIndex + 1) & 1023;
        {
            u32 random = gRandomIndex * 2;
            u16 *table;
            table = gUnk_0203B400;
            random += (u32)table;
            random = *(u16 *)random;
            direction -= 8;
            random &= 15;
            direction += random;
        }
        ContactSwarm_SpawnCharge(*(void **)((u8 *)p2 + 0x9d4), &spawn, direction, 20, zero, zero,
                                 zero);
        Sound_PlaySongChecked(234);
    }
    if (*((u8 *)p2 + 0x25d) & 2) {
        register int elapsed asm("r3") = tick;
        asm("" : "+r"(elapsed));
        if (elapsed > 40)
            p->finished = 1;
    }
}
