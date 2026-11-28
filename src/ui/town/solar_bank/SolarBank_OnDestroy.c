#include "global.h"
#include "ui/town/solar_bank_panel.h"
void Anim_Remove(struct AnimSprite *);
void Sprite_Remove(struct DisplaySequenceSprite *);
s32 SolarBank_OnDestroy(struct SolarBankPanel *panel)
{
    struct AnimSprite *sprites = &panel->label;
    s32 i;
    for (i = 0; i < 16; i++)
        Anim_Remove(&sprites[i]);
    for (i = 0; i < 2; i++)
        Sprite_Remove(&panel->sprites[i]);
    return 0;
}
