#include "global.h"
#include "scene_input.h"
#define SceneInventory_ResetCategoryPalette sub_081BE0F0
#define gScenePaletteBanks gUnk_030042E4
struct SceneInventoryCategoryMenu {
    u8 unknown0[0x13F8];
    struct SceneInputWidget categoryWidget;
    struct SceneInputWidget targetWidget;
};
void sub_081BE0F0(struct SceneInventoryCategoryMenu *p)
{
    u16 *palette = (u16 *)(gUnk_030042E4 + 0x7540);
    p->categoryWidget.palette = palette;
    p->categoryWidget.paletteBank = 0x3AA;
    p->targetWidget.palette = palette;
    p->targetWidget.paletteBank = 0x3AA;
}
