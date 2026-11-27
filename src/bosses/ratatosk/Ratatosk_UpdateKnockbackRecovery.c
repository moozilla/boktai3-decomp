#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
#include "bosses/ratatosk/ratatosk_particle.h"
#include "bosses/ratatosk/ratatosk_facing.h"
#include "engine/ram.h"
#include "scene/actor/scene_actor_components.h"

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
    v->x = x;
    v->y = 0;
    v->z = trunc12(gUnk_086149C4[angle & 255] * radius);
}

void Ratatosk_UpdateDirectionalAnimation(void);
void Ratatosk_UpdateDefeatSequence(void);
// confidence=low: recovers from knockback, choosing defeat when health is depleted.
void Ratatosk_UpdateKnockbackRecovery(struct Ratatosk *p, s32 tick)
{
    struct RatatoskParticleConfig params;
    struct ContactPosition spawn;
    struct Ratatosk *p2 = p;
    struct SceneActorBody *body = &p->body;
    u16 *scaleX;
    u16 *scaleZ;
    if (p->actionChanged) {
        s32 angle;
        p->actionChanged = 0;
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
    p2->frameDamage = 0;
    {
        Ratatosk_SetFlags(p, 12);
    }
    if (tick > 29) {
        *scaleX = 0;
        *scaleZ = 0;
        p2->scaleMode = 0;
        if (p->health <= 0 && !p->locked) {
            Ratatosk_StartReaction(p, Ratatosk_UpdateDefeatSequence, 15);
        } else {
            Ratatosk_StartReaction(p, Ratatosk_UpdateDirectionalAnimation, 1);
            p->actionFinished = 1;
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
            spawn = p->body.position;
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
            Ratatosk_SpawnAnimatedParticle(&params, &spawn);
        }
        {
            Ratatosk_SetFlags(p, 1);
        }
    }
}
