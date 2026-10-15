#include "global.h"
struct KeyframePaletteView {
    u8 unknown0[0x3A];
    u16 paletteIndex;
    u8 unknown3C[0xC];
    u8 *paletteData;
};
extern u8 *gUnk_030042E4;
void Anim_SelectPaletteBank(struct KeyframePaletteView *animation, u32 index)
{
    s32 selected;
    animation->paletteIndex = index;
    selected = animation->paletteIndex;
    animation->paletteData = gUnk_030042E4 + (selected << 5);
}
