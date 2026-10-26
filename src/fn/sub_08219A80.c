#include "global.h"
#include "render.h"
struct KeyframePaletteView {
    u8 unknown0[0x3A];
    u16 paletteIndex;
    u8 unknown3C[0xC];
    u8 *paletteData;
};
void Anim_SelectPaletteBank(struct KeyframePaletteView *animation, u32 index)
{
    s32 selected;
    animation->paletteIndex = index;
    selected = animation->paletteIndex;
    animation->paletteData = gPaletteBankData + (selected << 5);
}
