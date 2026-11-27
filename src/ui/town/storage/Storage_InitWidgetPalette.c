#include "global.h"
#include "ui/widgets/scene_input.h"
#include "ui/town/storage_screen.h"
#define gScenePaletteBanks gPaletteBankData
#define Palette_CopyColors Palette_CopyBank
struct StorageWidgetPalette {
    u16 colors[16];
    u8 frame;
};
void Palette_CopyBank(void *, const void *);
void Storage_InitWidgetPalette(struct StorageScreen *p,
                                      struct StorageWidgetPalette *palette)
{
    Palette_CopyBank(palette, gPaletteBankData + 0x7540);
    palette->frame = 0;
    p->labels[1].paletteBank = 0xE010;
    p->labels[1].palette = palette->colors;
    p->labels[0].paletteBank = 0xE010;
    p->labels[0].palette = palette->colors;
    p->controls[6].paletteBank = 0xE010;
    p->controls[6].palette = palette->colors;
}
