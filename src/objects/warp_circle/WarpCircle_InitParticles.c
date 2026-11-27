#include "objects/warp_circle/warp_circle.h"
void *AnimData_FindEntry(u32);
void ParticleSprite_Initialize(struct WarpCircleParticle *, void *, u32);
void Effect_SetOrigin(struct WarpCircleParticle *, s32, s32);
void Effect_SetPalette(struct WarpCircleParticle *, u32);
void SpriteDrawable_SelectTileFrame(struct WarpCircleParticle *, void *, u32);
void WarpCircle_InitParticles(struct WarpCircle *actor)
{
    s32 i;
    for (i = 0; i < 8; i++) {
        struct WarpCircleParticle *particle = &actor->particles[i];
        ParticleSprite_Initialize(particle, AnimData_FindEntry(0x1C1E), 1);
        Effect_SetOrigin(particle, -4, -4);
        Effect_SetPalette(particle, 1);
        SpriteDrawable_SelectTileFrame(particle, AnimData_FindEntry(0x1C1E), 2);
        particle->renderMode = 1;
        particle->priority = 20;
        particle->position = actor->world;
        actor->particles[i].enabled = 0;
    }
}
