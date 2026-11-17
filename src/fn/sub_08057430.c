#include "level_advance_display.h"
void LevelAdvanceDisplay_InitParticles(struct LevelAdvanceManager *manager)
{
    struct LevelAdvanceParticle *particle;
    s32 i;
    s32 scale;
    manager->particleResource = AnimData_FindEntry(0x1C1E);
    scale = -4;
    particle = manager->particles;
    for (i = 7; i >= 0; i--) {
        ParticleSprite_Initialize(&particle->sprite, manager->particleResource, 1);
        SpriteDrawable_SelectTileFrame(&particle->sprite, manager->particleResource, 0);
        Effect_SetOrigin(&particle->sprite, scale, scale);
        Effect_SetPalette(&particle->sprite, 1);
        particle++;
    }
}
