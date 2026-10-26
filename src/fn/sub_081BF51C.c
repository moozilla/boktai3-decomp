#include "global.h"
#include "scene_input.h"
#define gScenePaletteBanks gPaletteBankData
#define Palette_CopyColors Palette_CopyBank
struct SceneInventoryWidgetPalette {
    u16 colors[16];
    u8 frame;
};
struct SceneInventoryCategoryMenu {
    u8 unknown0[0x258];
    struct SceneInputWidget prompt;
    u8 unknown2B8[0x1140];
    struct SceneInputWidget categoryWidget;
    struct SceneInputWidget targetWidget;
};
void Palette_CopyBank(void *, const void *);
void SceneInventory_InitWidgetPalette(struct SceneInventoryCategoryMenu *p,
                                      struct SceneInventoryWidgetPalette *palette)
{
    Palette_CopyBank(palette, gPaletteBankData + 0x7540);
    palette->frame = 0;
    p->targetWidget.paletteBank = 0xE010;
    p->targetWidget.palette = palette->colors;
    p->categoryWidget.paletteBank = 0xE010;
    p->categoryWidget.palette = palette->colors;
    p->prompt.paletteBank = 0xE010;
    p->prompt.palette = palette->colors;
}
