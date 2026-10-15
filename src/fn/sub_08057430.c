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
        sub_08217EEC(particle, manager->particleResource, 0);
        sub_08217EC4(particle, scale, scale);
        sub_08217ECC(particle, 1);
        particle++;
    }
}
