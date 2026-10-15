#include "level_advance_display.h"
#define LevelAdvanceDisplay_StartLevelAnimation sub_080570BC
void LevelAdvanceDisplay_StartLevelAnimation(struct LevelAdvanceManager *manager)
{
    s32 i;
    manager->sprite.flags &= ~1;
    manager->sprite.frame = 0;
    manager->sprite.position = gUnk_02000580[0]->position;
    manager->sprite.position.world.y += 250;
    manager->frame = 0;
    manager->active = 1;
    manager->kind = 1;
    for (i = 0; i < 8; i++)
        sub_08217EEC(&manager->particles[i], manager->particleResource, 0);
}
