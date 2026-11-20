#include "ram.h"
#include "animated_particle.h"
#include "monster_particle_random.h"
#include "display_sequence_sprite.h"

struct AnimatedParticleSpawnParams {
    u16 type, flags, pad, speed;
    s16 vertical, verticalRange, horizontal, horizontalRange;
    u16 pitch, pitchRange, yaw, yawRange, verticalStart, verticalStartRange, verticalDecay,
        verticalDecayRange, horizontalStart, horizontalStartRange, horizontalDecay,
        horizontalDecayRange;
};
struct AnimatedParticleManager *AnimatedParticleManager_Create(int, int);
int Mod(int, int);

int AnimatedParticle_PriorityFromFlags(int);
void Anim_SetDurationScale(void *, int);
void Sprite_LinkToList(void *, int);
static inline int scale12(int a, int b)
{
    int x = a * b;
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}
static inline void AnimatedParticle_SetFrame(struct DisplaySequenceSprite *o, int f)
{
    o->frame = f;
}
int AnimatedParticles_Create(int count, struct ContactPosition *pos,
                             struct AnimatedParticleSpawnParams *c)
{
    int i, emitted;
    if (!gAnimatedParticleManager && !AnimatedParticleManager_Create(0, 0))
        return -1;
    emitted = 0;
    i = 0;
    for (; i <= 31; i++) {
        struct AnimatedParticle *p =
            &((struct AnimatedParticleManager *)gAnimatedParticleManager)->pieces[i];
        struct DisplaySequenceSprite *o = &p->sprite;
        if (!p->active) {
            int pitch, yaw;
            p->active = 1;
            p->type = c->type;
            p->timer = 0;
            if (!c->verticalRange)
                p->verticalScale = c->vertical;
            else
                p->verticalScale = c->vertical + Mod(ParticleRandom_ReadDirectSample(gRandomTable),
                                                     c->verticalRange);
            if (!c->horizontalRange)
                p->horizontalScale = c->horizontal;
            else
                p->horizontalScale =
                    c->horizontal +
                    Mod(ParticleRandom_ReadDirectSample(gRandomTable), c->horizontalRange);
            if (!c->pitchRange)
                pitch = c->pitch;
            else
                pitch =
                    c->pitch + Mod(ParticleRandom_ReadDirectSample(gRandomTable), c->pitchRange);
            if (!c->yawRange)
                yaw = c->yaw;
            else
                yaw = c->yaw + Mod(ParticleRandom_ReadDirectSample(gRandomTable), c->yawRange);
            if (!c->verticalStartRange)
                p->verticalStart = c->verticalStart;
            else
                p->verticalStart =
                    c->verticalStart +
                    Mod(ParticleRandom_ReadDirectSample(gRandomTable), c->verticalStartRange);
            if (!c->horizontalStartRange)
                p->horizontalStart = c->horizontalStart;
            else
                p->horizontalStart =
                    c->horizontalStart +
                    Mod(ParticleRandom_ReadDirectSample(gRandomTable), c->horizontalStartRange);
            if (c->flags & 1)
                o->flags |= 512;
            else {
                o->flags &= ~512;
                o->flags &= ~1024;
            }
            if (c->flags & 2)
                o->flags |= 16;
            else
                o->flags &= ~16;
            p->direction.x = gUnk_086149C4[(yaw + 64) & 255];
            p->direction.y = gUnk_086149C4[pitch & 255];
            p->direction.z = gUnk_086149C4[yaw & 255];
            p->velocity.x = scale12(p->direction.x, p->horizontalScale);
            p->velocity.y = scale12(p->direction.y, p->verticalScale);
            p->velocity.z = scale12(p->direction.z, p->horizontalScale);
            if (p->type == 5) {
                AnimData_Load(&p->resource, 0x61f9);
                AnimData_SelectPaletteBank(&p->resource, 0x29f);
                AnimatedParticle_SetFrame(&p->sprite, 32);
                Anim_SetSequence(
                    &p->anim,
                    ((struct AnimatedParticleManager *)gAnimatedParticleManager)->resources[1], 6,
                    0, 0);
            } else if (p->type == 6) {
                AnimData_Load(&p->resource, 0x61f9);
                AnimData_SelectPaletteBank(&p->resource, 0x2a7);
                AnimatedParticle_SetFrame(&p->sprite, 32);
                Anim_SetSequence(
                    &p->anim,
                    ((struct AnimatedParticleManager *)gAnimatedParticleManager)->resources[1], 6,
                    0, 0);
            } else if (p->type == 7) {
                AnimData_Load(&p->resource, 0x61f9);
                AnimData_SelectPaletteBank(&p->resource, 0x2a6);
                AnimatedParticle_SetFrame(&p->sprite, 45);
                Anim_SetSequence(
                    &p->anim,
                    ((struct AnimatedParticleManager *)gAnimatedParticleManager)->resources[1], 8,
                    0, 0);
            } else {
                AnimData_Load(&p->resource, 0x4384);
                AnimData_SelectPaletteBank(&p->resource, p->type + 182);
                Anim_SetSequence(
                    &p->anim,
                    ((struct AnimatedParticleManager *)gAnimatedParticleManager)->resources[0], 0,
                    0, 0);
            }
            DisplaySequence_StepSprite(o, &p->anim);
            o->position = *pos;
            o->priority = AnimatedParticle_PriorityFromFlags(c->flags);
            Anim_SetDurationScale(&p->anim, c->speed);
            Sprite_LinkToList(o, 0);
            if (emitted++ >= count)
                return 0;
        }
    }
    return -1;
}
