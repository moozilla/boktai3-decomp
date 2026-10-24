#include "global.h"
// MATCHING_CONCESSION: register-pinning

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Remaining differences: owner and random-source allocation, scale decay and state setup.
struct Vec {
    s16 x, y, z, pad;
};
struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct Actor {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[2];
    struct Vec position, velocity;
    u8 pad64[0x2c];
    u32 timer;
    s16 health;
    u8 pad96[6];
    u32 flags;
    u8 padA0[2];
    u8 mode, padA3;
    u32 modeTime;
    u8 facing, facing8, padAA, animationId, state, nextState, active, finished, padB0, changed,
        padB2, locked;
    u8 padB4[0x18];
    u8 requestedAngle;
    u8 padCD[0x33];
    void (*update)(void);
    void *callback;
    u8 pad108[0xc];
    s8 savedAngle;
    u8 pad115[0x67];
    u32 phase;
    u8 pad180[0x1ac];
    u8 animation[0x20];
    u8 animationData[0x20];
    struct Vec anchor;
    u8 pad374[0x58];
    struct Vec home;
    u8 pad3D4[0xa78];
    s16 selectedPoint, selectedTarget;
    u16 padE50;
    u16 scaleX, scaleZ, padE56, scaleStep;
    u8 padE5A[0x14];
    u16 motion;
    u8 motionFlag, padE71, targetAngle, phaseTimer;
    s16 hitTimer, terrainTimer;
};
#define H(o) (*(s16 *)((u8 *)p2 + (o)))
#define U(o) (*(u16 *)((u8 *)p2 + (o)))
#define B(o) (*(u8 *)((u8 *)p2 + (o)))
#define W(o) (*(u32 *)((u8 *)p2 + (o)))
extern u32 gUnk_03005304;
extern const u16 gUnk_086141BC[];
extern const s16 gUnk_086149C4[];
void SceneActors_StartDirectionalAnimation(void *, void *, void *, s32, s32, s32, s32);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, s32, s32, s32);
void SceneActors_StartAnimation(void *, void *, void *, s32, s32, s32);
void Sound_PlaySong(u32);
void Sound_StopSong(u32);
u8 Callback_InvokeActor(void *, void *);
s32 Math_Atan2(s32, s32);
static inline u32 randomA(void)
{
    gUnk_03005304 = (gUnk_03005304 + 1) & 1023;
    return gUnk_086141BC[gUnk_03005304];
}
static inline void facing(struct Actor *p, s32 angle)
{
    u32 v;
    s32 shift;
    p->angle = angle;
    shift = (p->angle + 0x60) >> 6;
    v = 3;
    v &= shift;
    p->facing = v;
    shift = (p->angle + 0x70) >> 5;
    v = 7;
    v &= shift;
    p->facing8 = v;
}
static inline s32 trunc12(s32 v)
{
    return v >= 0 ? v >> 12 : -((-v) >> 12);
}
static inline void polar(struct Vec *v, s32 angle, s32 radius)
{
    s32 x = trunc12(gUnk_086149C4[(angle + 64) & 255] * radius);
    s32 zero = 0;
    v->x = x;
    v->y = zero;
    v->z = trunc12(gUnk_086149C4[angle & 255] * radius);
}
static inline void sum(struct Vec *d, struct Vec *a, struct Vec *b)
{
    d->x = a->x + b->x;
    d->y = a->y + b->y;
    d->z = a->z + b->z;
}
void Ratatosk_InitAnimatedParticleParams(void *);
void Ratatosk_ConfigureAnimatedParticle(void *, s32, s32, u8, s32, s32, s32);
void sub_081FC26C(void *, struct Vec *);
void Ratatosk_UpdateDirectionalAnimation(void);
void Ratatosk_UpdateDefeatSequence(void);
static inline void decay(u16 *v)
{
    u16 n = *v;
    if ((s16)*v > 23)
        *v = n - 24;
    else
        *v = 0;
}
static inline void setState(struct Actor *p, void (*update)(void), u8 state)
{
    p->animationId = state;
    p->update = update;
    p->timer = 0;
    p->changed = 1;
    p->mode = 0;
    p->modeTime = 0;
}
// confidence=low: recovers from knockback, choosing defeat when health is depleted.
#define Ratatosk_UpdateKnockbackRecovery sub_081F5290
void Ratatosk_UpdateKnockbackRecovery(struct Actor *p, s32 elapsed)
{
    s32 tick = elapsed;
    u8 params[0x1c];
    struct Vec spawn;
    struct Actor *p2 = p;
    u8 *sprite = ((u8 *)p) + 0x48;
    u16 *scaleX;
    u16 *scaleZ;
    register u32 *flags asm("r9");
    if (p->changed) {
        s32 angle;
        p->changed = 0;
        if (p->locked)
            angle = p->savedAngle;
        else
            angle = p->targetAngle + 128;
        do {
            facing(p, angle);
        } while (0);
        SceneActors_StartDirectionalAnimation(p, p2->animationData, p2->animation, 29, 0, 1, 4);
        Sound_PlaySong(0x55e);
    }
    if (tick <= 15) {
        if (!p->locked) {
            u8 *angleAddress = &p2->targetAngle;
            s32 duration = 16;
            struct Vec *v = (struct Vec *)(sprite + 20);
            u32 angle = *angleAddress;
            s32 speed = duration - tick;
            polar(v, angle, speed);
        } else {
            s32 angle = p->savedAngle;
            s32 duration = 16;
            struct Vec *v = (struct Vec *)(sprite + 20);
            polar(v, angle + 128, duration - tick);
        }
    }
    scaleX = &p2->scaleX;
    decay(scaleX);
    scaleZ = &p2->scaleZ;
    decay(scaleZ);
    {
        typeof(p2->phaseTimer) *q1 = &p2->phaseTimer;
        u8 z1 = 8;
        *q1 = z1;
        p2->motion = 0;
        {
            u32 mask = 12;
            flags = &p->flags;
            *flags |= mask;
        }
        if (tick > 29) {
            *scaleX = 0;
            *scaleZ = 0;
            p2->scaleStep = 0;
            if ((p->health <= 0) && (!p->locked)) {
                setState(p, Ratatosk_UpdateDefeatSequence, 15);
            } else {
                setState(p, Ratatosk_UpdateDirectionalAnimation, 1);
                p->nextState = 1;
            }
        } else {
            register u32 one asm("r12") = 1;
            asm("" : "+r"(one));
            if (tick & one) {
                register u32 speed asm("r7") = randomA() & 7;
                register u32 variant asm("r6") = randomA() & one;
                register u32 angle asm("r8") = (u8)randomA();
                register u32 drift asm("r4") = randomA() & 15;
                struct Vec *position = &spawn;
                spawn = p->position;
                position->x -= 128;
                position->y += 128;
                variant <<= 12;
                drift <<= 12;
                Ratatosk_InitAnimatedParticleParams(params);
                Ratatosk_ConfigureAnimatedParticle(params, (randomA() & 31) + 52, speed, angle, 1,
                                                   variant, drift);
                sub_081FC26C(params, position);
            }
            {
                u32 mask = 1;
                {
                    s32 v1 = (*flags) | mask;
                    *flags = v1;
                }
            }
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081F5290);
#endif
