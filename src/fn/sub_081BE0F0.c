#include "global.h"
#include "scene_input.h"
#define gScenePaletteBanks gPaletteBankData
struct SceneInventoryCategoryMenu {
    u8 unknown0[0x13F8];
    struct SceneInputWidget categoryWidget;
    struct SceneInputWidget targetWidget;
};
void SceneInventory_ResetCategoryPalette(struct SceneInventoryCategoryMenu *p)
{
    u16 *palette = (u16 *)(gPaletteBankData + 0x7540);
    p->categoryWidget.palette = palette;
    p->categoryWidget.paletteBank = 0x3AA;
    p->targetWidget.palette = palette;
    p->targetWidget.paletteBank = 0x3AA;
}
