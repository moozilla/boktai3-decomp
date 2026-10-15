#include "global.h"

struct SequencePaletteView {
    u8 unknown0[6];
    u16 paletteIndex;
    u8 unknown8[4];
    u32 paletteData;
};
extern u16 gUnk_030042E0;
extern u8 *gUnk_030042E4;

void AnimData_SelectPaletteBank(struct SequencePaletteView *o, s32 i)
{
    if (i < gUnk_030042E0) {
        o->paletteIndex = i;
        o->paletteData = (u32)gUnk_030042E4 + (o->paletteIndex << 5);
    }
}
