#include "level_advance_display.h"
void LevelAdvanceDisplay_InitSprite(struct LevelAdvanceManager *manager)
{
    u8 *graphics = manager->graphics;
    AnimData_Load(graphics, 0x1C1D);
    Sprite_Init(&manager->sprite, graphics, 0);
    AnimData_SelectPaletteBank(graphics, 1);
    manager->sprite.flags |= 1;
    manager->sprite.priority = 1;
}
