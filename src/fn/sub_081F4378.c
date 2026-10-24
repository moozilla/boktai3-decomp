#include "global.h"

#ifdef NONMATCHING
// fix.py best after the inlined-insn fix
// Remaining differences: register allocation, pointer lifetimes and shared block placement.
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
    d->x = b->x + a->x;
    d->y = a->y + b->y;
    d->z = a->z + b->z;
}
void Ratatosk_InitAnimatedParticleParams(void *);
void Ratatosk_ConfigureAnimatedParticle(void *, s32, s32, u8, s32, s32, s32);
void sub_081FC26C(void *, struct Vec *);
// confidence=low: staged attack animation culminates in a three-projectile volley.
struct Vec *Player_GetEffectivePosition(s32);
void Projectile_InitParameters(void *);
void Projectile_SetLaunchParameters(void *, u16, u32, u8, u32);
void Projectile_SetSpriteParameters(void *, u32, u32, u32);
void Projectile_SetScriptProcedure(void *, u32, u16);
void Projectile_SetCollisionParameters(void *, s32, u32, s32, s32, s32);
void Projectile_SetBehaviorMode(void *, u16);
void Projectile_SpawnAtPosition(void *, struct Vec *);
s32 SceneProjectile_ConsumeHitEvent(void);
inline static void polar_2(u8 a1, struct Vec *a0, int a2)
{
    polar(a0, a1, a2);
}
inline static void polar_2_1(s16 a1, struct Vec *a0)
{
    polar(a0, a1, 180);
}

void Ratatosk_UpdateProjectileVolley(struct Actor *p, s32 tick)
{
    int new_var;
    u8 params[0x58];
    struct Vec delta;
    struct Vec spawn;
    s16 k2;
    unsigned char new_var2;
    struct Actor *p2 = p;
    new_var2 = 15;
    if (p->changed) {
        u32 zero = 0;
        p->changed = zero;
        facing(p, p->requestedAngle);
        SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 17, 1, 4, 4);
        p->phase = zero;
    }
    switch (p->phase) {
        case 0:
            if (tick > 9) {
                SceneActors_StartDirectionalAnimation(p, p2->animationData, p2->animation, 18, 1, 4,
                                                      4);
                p2->phase++;
            }
            break;

        case 1:
            if (tick > 33) {
                SceneActors_StartDirectionalAnimation(p, p2->animationData, p2->animation, 19, 1, 4,
                                                      4);
                p2->phase++;
            }
            break;

        case 2:
            if (tick > 36) {
                SceneActors_StartDirectionalAnimation(p, p2->animationData, p2->animation, 20, 1, 4,
                                                      4);
                p2->phase++;
            }
            break;

        case 3:
            if (tick > 42) {
                struct Vec *target = Player_GetEffectivePosition(0);
                struct Vec *anchor = &p2->anchor;
                u8 angle = Math_Atan2(target->x - anchor->x, target->z - anchor->z);
                u8 facingAngle = (p->facing << 6) - 64;
                s32 difference = facingAngle - angle;
                s32 i;
                new_var = 16;
                if (difference > 128)
                    difference = 255 - difference;
                else if (difference < (-128))
                    difference += 255;
                if (difference > 32)
                    angle = facingAngle - (k2 = 32);
                else if (difference < (-32)) {
                    angle = 32;
                    angle |= facingAngle;
                }
                polar_2_1((u8)((p->facing << 6) - 64), &delta);
                sum(&spawn, &delta, &p2->anchor);
                spawn.y += 200;
                angle -= new_var;
                Sound_PlaySong(0x568);
                {
                    struct Vec *spawnPtr = &spawn;
                    i = 2;
                    do {
                        Projectile_InitParameters(params);
                        Projectile_SetLaunchParameters(params, 0, 150, angle, 24);
                        Projectile_SetSpriteParameters(params, 0x8e5a, 0, 498);
                        Projectile_SetScriptProcedure(params, 0x4363, 0);
                        Projectile_SetCollisionParameters(params, 0, 0, 30, new_var2, 30);
                        Projectile_SetBehaviorMode(params, 1);
                        Projectile_SpawnAtPosition(params, spawnPtr);
                        angle += new_var;
                    } while ((--i) >= 0);
                }
                p2->phase++;
            }
            break;

        case 4:
            if (SceneProjectile_ConsumeHitEvent() && (p2->hitTimer == 0)) {
                p2->hitTimer = 120;
                p->finished = 1;
            }
            break;
    }

    if (tick > 119) {
        p2->phase = 0;
        p->finished = 1;
    }
    Callback_InvokeActor(p, p->callback);
}

#else
INCLUDE_ASM("asm/nonmatching", Ratatosk_UpdateProjectileVolley);
#endif
