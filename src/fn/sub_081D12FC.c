#include "scene_npc_transition.h"

extern const s16 gUnk_086149C4[];
void SpriteDrawable_SelectTileFrame(void *, void *, s32);

static inline s32 Scale12(s32 value)
{
    return value >= 0 ? value >> 12 : -(-value >> 12);
}
static inline void MakeVelocity(struct ContactPosition *out, s32 angle, s32 speed)
{
    out->x = Scale12(speed * gUnk_086149C4[(angle + 64) & 255]);
    out->y = 0;
    out->z = Scale12(speed * gUnk_086149C4[angle & 255]);
}
static inline void AddVelocity(struct ContactPosition *velocity, struct ContactPosition *position)
{
    position->x += velocity->x;
    position->y += velocity->y;
    position->z += velocity->z;
}
void SceneNpc_UpdateTransitionParticles(struct TransitionEffect *effect)
{
    struct ContactPosition velocity;
    struct ContactPosition *delta = &velocity;
    struct TransitionParticle *particle = effect->particles;
    u8 *age = &particle->motion.age;
    struct TransitionMotion *motion = &particle->motion;
    struct TransitionParticle *drawable = particle;
    s32 count = 7;
    do {
        if (motion->active) {
            if (motion->angle >= 0) {
                MakeVelocity(&velocity, motion->angle, 15);
                AddVelocity(delta, &motion->position);
            }
            motion->verticalVelocity--;
            motion->position.y += motion->verticalVelocity;
            if (motion->age == 8)
                SpriteDrawable_SelectTileFrame(drawable, effect->graphics, 3);
            {
                u32 nextAge = *age + 1;
                *age = nextAge;
                nextAge &= 255;
                if (nextAge >= motion->lifetime) {
                    particle->flags |= 1;
                    motion->active = 0;
                }
            }
        }
        particle++;
        age += sizeof(*particle);
        motion = (struct TransitionMotion *)((u8 *)motion + sizeof(*particle));
        drawable++;
    } while (--count >= 0);
}
