#include "level_advance_display.h"
#define LevelAdvanceDisplay_StartSecondaryAnimation sub_08057114
void LevelAdvanceDisplay_StartSecondaryAnimation(struct LevelAdvanceManager *manager,
                                                 s32 secondaryKind)
{
    manager->sprite.flags &= ~1;
    manager->secondaryKind = secondaryKind;
    manager->sprite.frame = 3;
    manager->sprite.position = gUnk_02000580[0]->position;
    manager->sprite.position.world.y += 250;
    manager->frame = 0;
    manager->active = 1;
    manager->kind = 2;
}
