#include "global.h"
#include "ui/widgets/scene_input.h"
#include "ui/town/storage_screen.h"
/* Tentative role names; the offsets are established by palette consumers. */
void Storage_SetCategoryPalette(struct StorageScreen *screen, u16 *colors)
{
    u16 *palette = (u16 *)(gPaletteBankData + 0x7540);
    screen->labels[0].palette = palette;
    screen->labels[0].paletteBank = 0x3AA;
    screen->labels[1].palette = colors;
    screen->labels[1].paletteBank = 0x8000;
}
