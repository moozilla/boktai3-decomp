#include "global.h"

struct TransitionVec {
    s16 x, y, z;
};
struct TransitionMotion {
    struct TransitionVec position;
    u8 unk_06[8];
    s16 active;
    s16 angle;
    s16 verticalVelocity;
    u8 age;
    u8 lifetime;
};
struct TransitionParticle {
    u32 flags;
    u8 unk_04[0x14];
    struct TransitionMotion motion;
};
/* Tentative view of the NPC transition effect and its eight particles. */
struct TransitionEffect {
    u8 unk_00[0x78];
    void *graphics;
    struct TransitionParticle particles[8];
};
extern const s16 gUnk_086149C4[];
void SpriteDrawable_SelectTileFrame(void *, void *, s32);

static inline s32 Scale12(s32 value)
{
    return value >= 0 ? value >> 12 : -(-value >> 12);
}
static inline void MakeVelocity(struct TransitionVec *out, s32 angle, s32 speed)
{
    out->x = Scale12(speed * gUnk_086149C4[(angle + 64) & 255]);
    out->y = 0;
    out->z = Scale12(speed * gUnk_086149C4[angle & 255]);
}
static inline void AddVelocity(struct TransitionVec *velocity, struct TransitionVec *position)
{
    position->x += velocity->x;
    position->y += velocity->y;
    position->z += velocity->z;
}
void SceneNpc_UpdateTransitionParticles(void *arg0)
{
    struct TransitionEffect *effect = arg0;
    struct TransitionVec velocity;
    struct TransitionVec *delta = &velocity;
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
