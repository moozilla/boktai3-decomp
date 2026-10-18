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
        sub_08217DE0(particle, manager->particleResource, 1);
        SpriteDrawable_SelectTileFrame(particle, manager->particleResource, 0);
        Effect_SetOrigin(particle, scale, scale);
        Effect_SetPalette(particle, 1);
        particle++;
    }
}
