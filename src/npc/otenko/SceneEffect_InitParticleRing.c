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
    s32 i;
    effect->origin = *origin;
    effect->frame = 0;
    AnimData_Load(&effect->animation, 0x3641);
    Sprite_Init(&effect->sprite, &effect->animation, 2);
    effect->sprite.position = *origin;
    effect->sprite.palette = 0x40;
    effect->sprite.paletteVariant = 0x40;
    effect->sprite.priority = 3;
    effect->graphics = AnimData_FindEntry(0x1C1E);
    for (i = 0; i < 8; i++) {
        struct TransitionParticle *particle = &effect->particles[i];
        ParticleSprite_Initialize(particle, effect->graphics, 1);
        Effect_SetOrigin(particle, -4, -4);
        SpriteDrawable_SelectTileFrame(particle, effect->graphics, 2);
        Effect_SetPalette(particle, 1);
        particle->motion.active = 0;
    }
    effect->state = 0;
    effect->stateFrame = 0;
    effect->enter = 1;
    return 0;
}
