#include "npc/otenko/scene_npc_transition.h"

void AnimData_Load(void *, s32);
void Sprite_Init(void *, void *, s32);
void *AnimData_FindEntry(s32);
void ParticleSprite_Initialize(void *, void *, s32);
void Effect_SetOrigin(void *, s32, s32);
void SpriteDrawable_SelectTileFrame(void *, void *, s32);
void Effect_SetPalette(void *, s32);
s32 SceneEffect_InitParticleRing(struct TransitionEffect *effect,
                                 const struct ContactPosition *origin)
{
    struct SceneAnimation *anim;
    struct TransitionParticle *particle;
    s32 i, neg;
    effect->origin = *origin;
    effect->frame = 0;
    anim = &effect->animation;
    AnimData_Load(anim, 0x3641);
    Sprite_Init(&effect->sprite, anim, 2);
    effect->sprite.position = *origin;
    effect->sprite.palette = 0x40;
    effect->sprite.paletteVariant = 0x40;
    effect->sprite.priority = 3;
    effect->graphics = AnimData_FindEntry(0x1C1E);
    neg = -4;
    particle = effect->particles;
    i = 7;
    do {
        ParticleSprite_Initialize(particle, effect->graphics, 1);
        Effect_SetOrigin(particle, neg, neg);
        SpriteDrawable_SelectTileFrame(particle, effect->graphics, 2);
        Effect_SetPalette(particle, 1);
        particle->motion.active = 0;
        particle++;
    } while (--i >= 0);
    effect->state = 0;
    effect->stateFrame = 0;
    effect->enter = 1;
    return 0;
}
