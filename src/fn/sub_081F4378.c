#include "global.h"

#ifdef NONMATCHING
// fuzzy: 80.8%
// Remaining differences: pointer allocation, polar-vector temporaries and instruction order.
#define Ratatosk_UpdateProjectileVolley sub_081F4378
struct Actor {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[0x56];
    u8 facing, facing8;
    u8 padAA[5];
    u8 finished, padB0, changed;
    u8 padB2[0x1a];
    u8 requestedAngle;
    u8 padCD[0x37];
    void *callback;
    u8 pad108[0x74];
    u32 volleyState;
    u8 pad180[0x1ac];
    u8 animation[0x20];
    u8 animationData[0x20];
    s16 x, y, z, pad372;
    u8 pad374[0xb00];
    s16 cooldown;
};
struct Vec {
    s16 x, y, z, pad;
};
extern const s16 gUnk_086149C4[];
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
void Sound_PlaySong(u32);
struct Vec *Player_GetEffectivePosition(int);
s32 Math_Atan2(s32, s32);
void Projectile_InitParameters(void *);
void Projectile_SetLaunchParameters(void *, int, int, int, int);
void Projectile_SetSpriteParameters(void *, int, int, int);
void Projectile_SetScriptProcedure(void *, int, int);
void Projectile_SetCollisionParameters(void *, int, int, int, int, int);
void Projectile_SetBehaviorMode(void *, int);
void sub_081FD01C(void *, void *);
s32 SceneProjectile_ConsumeHitEvent(void);
void Callback_InvokeActor(void *, void *);
static inline void facing(struct Actor *p, int angle)
{
    u32 v;
    int shifted;
    p->angle = angle;
    shifted = (p->angle + 0x60) >> 6;
    v = 3;
    v &= shifted;
    p->facing = v;
    shifted = (p->angle + 0x70) >> 5;
    v = 7;
    v &= shifted;
    p->facing8 = v;
}
static inline int trunc12(int v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
static inline void polar(struct Vec *v, u32 angle, int speed)
{
    v->x = trunc12(speed * gUnk_086149C4[(angle + 64) & 255]);
    v->y = 0;
    v->z = trunc12(speed * gUnk_086149C4[angle]);
}
// MATCHING_CONCESSION: register-pinning
static inline void sum(struct Vec *a, struct Vec *b, struct Vec *c)
{
    a->x = b->x + c->x;
    a->y = b->y + c->y;
    a->z = b->z + c->z;
}
void Ratatosk_UpdateProjectileVolley(struct Actor *input, s32 frame)
{
    struct Actor *p = input;
    struct Actor *q = p;
    u32 params[22];
    struct Vec offset, position;
    if (p->changed) {
        p->changed = 0;
        facing(p, p->requestedAngle);
        SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 17, 1, 4, 4);
        p->volleyState = 0;
    }
    switch (p->volleyState) {
        case 0:
            if (frame > 9) {
                SceneActors_StartDirectionalAnimation(p, q->animationData, q->animation, 18, 1, 4,
                                                      4);
                goto advance;
            }
            break;
        case 1:
            if (frame > 33) {
                SceneActors_StartDirectionalAnimation(p, q->animationData, q->animation, 19, 1, 4,
                                                      4);
                goto advance;
            }
            break;
        case 2:
            if (frame > 36) {
                SceneActors_StartDirectionalAnimation(p, q->animationData, q->animation, 20, 1, 4,
                                                      4);
                goto advance;
            }
            break;
        case 3:
            if (frame > 42) {
                struct Vec *target = Player_GetEffectivePosition(0);
                struct Vec *origin = (struct Vec *)&q->x;
                u32 angle = (u8)Math_Atan2(target->x - origin->x, target->z - origin->z);
                u32 base = (u8)((p->facing << 6) - 64);
                s32 delta = base - angle;
                int i;
                if (delta > 128)
                    delta = 255 - delta;
                else if (delta < -128)
                    delta += 255;
                if (delta > 32)
                    angle = (u8)(base - 32);
                else if (delta < -32)
                    angle = base | 32;
                polar(&offset, (u8)((p->facing << 6) - 64), 180);
                sum(&position, &offset, (struct Vec *)&q->x);
                {
                    struct Vec *v = &position;
                    v->y += 200;
                }
                angle = (u8)(angle - 16);
                Sound_PlaySong(0x568);
                {
                    register struct Vec *spawn asm("r8") = &position;
                    for (i = 2; i >= 0; i--) {
                        Projectile_InitParameters(params);
                        Projectile_SetLaunchParameters(params, 0, 150, angle, 24);
                        Projectile_SetSpriteParameters(params, 0x8e5a, 0, 498);
                        Projectile_SetScriptProcedure(params, 0x4363, 0);
                        Projectile_SetCollisionParameters(params, 0, 0, 30, 15, 30);
                        Projectile_SetBehaviorMode(params, 1);
                        sub_081FD01C(params, spawn);
                        angle = (u8)(angle + 16);
                    }
                }
            advance:
                q->volleyState++;
            }
            break;
        case 4:
            if (SceneProjectile_ConsumeHitEvent() && q->cooldown == 0) {
                q->cooldown = 120;
                p->finished = 1;
            }
            break;
    }
    if (frame > 119) {
        q->volleyState = 0;
        p->finished = 1;
    }
    Callback_InvokeActor(p, p->callback);
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081F4378);
#endif
