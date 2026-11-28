#include "global.h"
#include "ui/town/solar_bank_panel.h"
void SpriteMenu_HideAll(struct SolarBankPanel *panel)
{
    s32 i;
    struct AnimSprite *sprites = &panel->label;
    for (i = 0; i < 16; i++)
        sprites[i].flags |= 1;
    for (i = 0; i < 2; i++)
        panel->sprites[i].flags |= 1;
}
