#include "menus/level_advance_display/level_advance_display.h"
void LevelAdvanceDisplay_InitParticles(struct LevelAdvanceManager *manager)
{
    struct LevelAdvanceParticle *particle;
    s32 i;
    manager->particleResource = AnimData_FindEntry(0x1C1E);
    for (i = 0; i < 8; i++) {
        particle = &manager->particles[i];
        ParticleSprite_Initialize(&particle->sprite, manager->particleResource, 1);
        SpriteDrawable_SelectTileFrame(&particle->sprite, manager->particleResource, 0);
        Effect_SetOrigin(&particle->sprite, -4, -4);
        Effect_SetPalette(&particle->sprite, 1);
    }
}
