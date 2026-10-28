#include "level_advance_display.h"
void LevelAdvanceDisplay_StartSecondaryAnimation(struct LevelAdvanceManager *manager,
                                                 s32 secondaryKind)
{
    manager->sprite.flags &= ~1;
    manager->secondaryKind = secondaryKind;
    manager->sprite.frame = 3;
    manager->sprite.position = gPlayers[0]->position;
    manager->sprite.position.world.y += 250;
    manager->frame = 0;
    manager->active = 1;
    manager->kind = 2;
}
