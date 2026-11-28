#include "npc/otenko/scene_npc_transition.h"
#include "engine/math/math_shift.h"

extern const s16 gUnk_086149C4[];
void SpriteDrawable_SelectTileFrame(void *, void *, s32);

static inline void MakeVelocity(struct ContactPosition *out, s32 angle, s32 speed)
{
    out->x = Math_ShiftTowardZero(speed * gUnk_086149C4[(angle + 64) & 255], 12);
    out->y = 0;
    out->z = Math_ShiftTowardZero(speed * gUnk_086149C4[angle & 255], 12);
}
static inline void AddVelocity(struct ContactPosition *velocity, struct ContactPosition *position)
{
    position->x += velocity->x;
    position->y += velocity->y;
    position->z += velocity->z;
}
void Otenko_UpdateTransitionParticles(struct TransitionEffect *effect)
{
    struct ContactPosition velocity;
    s32 i;
    for (i = 0; i < 8; i++) {
        struct TransitionParticle *particle = &effect->particles[i];
        if (particle->motion.active) {
            if (particle->motion.angle >= 0) {
                MakeVelocity(&velocity, particle->motion.angle, 15);
                AddVelocity(&velocity, &particle->motion.position);
            }
            particle->motion.verticalVelocity--;
            particle->motion.position.y += particle->motion.verticalVelocity;
            if (particle->motion.age == 8)
                SpriteDrawable_SelectTileFrame(particle, effect->graphics, 3);
            if (++particle->motion.age >= particle->motion.lifetime) {
                particle->flags |= 1;
                particle->motion.active = 0;
            }
        }
    }
}
