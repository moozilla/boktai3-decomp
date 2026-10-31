#include "global.h"
#include "ratatosk_facing.h"
#include "ram.h"
#include "scene_actor_components.h"

struct Actor {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[2];
    struct ContactPosition position, velocity;
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
    struct ContactPosition anchor;
    u8 pad374[0x58];
    struct ContactPosition home;
    u8 pad3D4[0xa78];
    s16 selectedPoint, selectedTarget;
    u16 padE50;
    u16 scaleX, scaleZ;
    u16 padE56, scaleStep;
    u8 padE5A[0x14];
    u16 motion;
    u8 motionFlag, padE71, targetAngle, phaseTimer;
    s16 hitTimer, terrainTimer;
};
extern const u16 gUnk_086141BC[];
extern const s16 gUnk_086149C4[];
void SceneActors_StartDirectionalAnimation(void *, void *, void *, s32, s32, s32, s32);
void Sound_PlaySong(u32);
static inline u32 randomA(void)
{
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
    return gUnk_086141BC[gEffectRandomTableIndex];
}
static inline s32 trunc12(s32 v)
{
    return v >= 0 ? v >> 12 : -((-v) >> 12);
}
static inline void polar(struct ContactPosition *v, s32 angle, s32 radius)
{
    s32 x = trunc12(gUnk_086149C4[(angle + 64) & 255] * radius);
    s32 zero = 0;
    v->x = x;
    v->y = zero;
    v->z = trunc12(gUnk_086149C4[angle & 255] * radius);
}
struct AnimatedParticleParams {
    u16 kind, animation;
    u8 reserved4[2];
    u8 angle, reserved7;
    u32 speed, variant, drift;
    u32 field14, active;
};
void Ratatosk_InitAnimatedParticleParams(struct AnimatedParticleParams *);
void Ratatosk_ConfigureAnimatedParticle(struct AnimatedParticleParams *, u16, u32, u8, u32, u32,
                                        u32);
void sub_081FC26C(struct AnimatedParticleParams *, struct ContactPosition *);
void Ratatosk_UpdateDirectionalAnimation(void);
void Ratatosk_UpdateDefeatSequence(void);
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
void Ratatosk_UpdateKnockbackRecovery(struct Actor *p, s32 tick)
{
    struct AnimatedParticleParams params;
    struct ContactPosition spawn;
    struct Actor *p2 = p;
    struct SceneActorBody *body = (struct SceneActorBody *)((u8 *)p + 0x48);
    u16 *scaleX;
    u16 *scaleZ;
    if (p->changed) {
        s32 angle;
        p->changed = 0;
        if (p->locked)
            angle = p->savedAngle;
        else
            angle = p->targetAngle + 128;
        RATATOSK_SET_FACING(p, angle);
        SceneActors_StartDirectionalAnimation(p, p2->animationData, p2->animation, 29, 0, 1, 4);
        Sound_PlaySong(0x55e);
    }
    if (tick <= 15) {
        if (!p->locked)
            polar(&body->motion, p2->targetAngle, 16 - tick);
        else
            polar(&body->motion, p->savedAngle + 128, 16 - tick);
    }
    {
        u16 *value = &p2->scaleX;
        u16 old = *value;
        s32 current = (s16)*value;
        scaleX = value;
        if (current > 23)
            *scaleX = old - 24;
        else
            *scaleX = 0;
    }
    {
        u16 *value = &p2->scaleZ;
        u16 old = *value;
        s32 current = (s16)*value;
        scaleZ = value;
        if (current > 23)
            *scaleZ = old - 24;
        else
            *scaleZ = 0;
    }
    p2->phaseTimer = 8;
    p2->motion = 0;
    {
        u32 mask = 12;
        p->flags |= mask;
    }
    if (tick > 29) {
        *scaleX = 0;
        *scaleZ = 0;
        p2->scaleStep = 0;
        if (p->health <= 0 && !p->locked) {
            setState(p, Ratatosk_UpdateDefeatSequence, 15);
        } else {
            setState(p, Ratatosk_UpdateDirectionalAnimation, 1);
            p->finished = 1;
        }
    } else {
        if (tick & 1) {
            struct ContactPosition *position;
            u32 speed;
            u8 angle;
            s32 variant;
            s32 drift;
            speed = randomA() & 7;
            variant = randomA() & 1;
            angle = (u8)randomA();
            drift = randomA() & 15;
            position = &spawn;
            spawn = p->position;
            position->x -= 128;
            position->y += 128;
            /* Sign-symmetric fixed-point conversions. The samples are never negative,
             * but combine folds the tests only after cse has split the block there. */
            if (variant < 0)
                variant = -((-variant) << 12);
            else
                variant <<= 12;
            if (drift < 0)
                drift = -((-drift) << 12);
            else
                drift <<= 12;
            Ratatosk_InitAnimatedParticleParams(&params);
            Ratatosk_ConfigureAnimatedParticle(&params, (randomA() & 31) + 52, speed, angle, 1,
                                               variant, drift);
            sub_081FC26C(&params, &spawn);
        }
        {
            u32 bit = 1;
            p->flags |= bit;
        }
    }
}
