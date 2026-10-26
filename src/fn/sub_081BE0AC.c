#include "global.h"
#include "scene_input.h"
#define gScenePaletteBanks gPaletteBankData
/* Tentative role names; the offsets are established by palette consumers. */
struct SceneInventoryCategoryMenu {
    u8 unknown0[0x13F8];
    struct SceneInputWidget categoryWidget;
    struct SceneInputWidget targetWidget;
};
void SceneInventory_SetCategoryPalette(struct SceneInventoryCategoryMenu *p, u16 *colors)
{
    u16 *palette = (u16 *)(gPaletteBankData + 0x7540);
    p->categoryWidget.palette = palette;
    p->categoryWidget.paletteBank = 0x3AA;
    p->targetWidget.palette = colors;
    p->targetWidget.paletteBank = 0x8000;
}
