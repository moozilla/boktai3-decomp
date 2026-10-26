#include "global.h"
#include "render.h"

struct SequencePaletteView {
    u8 unknown0[6];
    u16 paletteIndex;
    u8 unknown8[4];
    u32 paletteData;
};

void AnimData_SelectPaletteBank(struct SequencePaletteView *o, s32 i)
{
    if (i < gPaletteBankCount) {
        o->paletteIndex = i;
        o->paletteData = (u32)gPaletteBankData + (o->paletteIndex << 5);
    }
}
