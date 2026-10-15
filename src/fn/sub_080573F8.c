#include "level_advance_display.h"
#define LevelAdvanceDisplay_InitSprite sub_080573F8
void LevelAdvanceDisplay_InitSprite(struct LevelAdvanceManager *manager)
{
    u8 *graphics = manager->graphics;
    sub_082151E4(graphics, 0x1C1D);
    sub_082144A4(&manager->sprite, graphics, 0);
    sub_08215284(graphics, 1);
    manager->sprite.flags |= 1;
    manager->sprite.priority = 1;
}
