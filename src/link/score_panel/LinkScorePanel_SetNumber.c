#include "global.h"
#include "ui/save/save_menu.h"
void AnimSprite_SelectGlyph(struct AnimSprite *, struct AnimSpriteResource *, u16);

void LinkScorePanel_SetNumber(struct SaveMenu *menu, s32 row, s32 value)
{
    s32 slot = row * 4 + 1;
    u32 glyph = 0x3e;
    while (value > 999) {
        glyph++;
        value -= 1000;
    }
    AnimSprite_SelectGlyph(&menu->widgets[slot], &menu->widgetResource, glyph);
    glyph = 0x3e;
    while (value > 99) {
        glyph++;
        value -= 100;
    }
    AnimSprite_SelectGlyph(&menu->widgets[slot + 1], &menu->widgetResource, glyph);
    glyph = 0x3e;
    while (value > 9) {
        glyph++;
        value -= 10;
    }
    AnimSprite_SelectGlyph(&menu->widgets[slot + 2], &menu->widgetResource, glyph);
    glyph = 0x3e;
    while (value > 0) {
        glyph++;
        value -= 1;
    }
    AnimSprite_SelectGlyph(&menu->widgets[slot + 3], &menu->widgetResource, glyph);
}
