#include "global.h"
#include "ui/widgets/scene_input.h"
#include "ui/town/storage_screen.h"
void Storage_ResetCategoryPalette(struct StorageScreen *screen)
{
    u16 *palette = (u16 *)(gPaletteBankData + 0x7540);
    screen->labels[0].palette = palette;
    screen->labels[0].paletteBank = 0x3AA;
    screen->labels[1].palette = palette;
    screen->labels[1].paletteBank = 0x3AA;
}
