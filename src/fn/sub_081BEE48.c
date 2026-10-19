#include "global.h"
#include "scene_input.h"
struct SceneInventoryCategoryMenu {
    u8 unknown0[0x258];
    struct SceneInputWidget primaryWidget;
    u8 unknown2B8[0x1200];
    struct SceneInputWidget categoryWidget;
    u8 unknown1518[0x2B0];
    u16 paletteTarget[16];
};
void Storage_RefreshCategoryWidgets(struct SceneInventoryCategoryMenu *);
void Storage_RefreshSelectedItemDetails(struct SceneInventoryCategoryMenu *);
void SceneInventory_SetCategoryPalette(struct SceneInventoryCategoryMenu *, u16 *);
void SceneInventory_UpdateCategoryBrowse(struct SceneInventoryCategoryMenu *);
void SceneInventory_SetState(struct SceneInventoryCategoryMenu *,
                             void (*)(struct SceneInventoryCategoryMenu *), u32);
static inline void EnableWidget(struct SceneInputWidget *widget)
{
    widget->flags |= 1;
}
void SceneInventory_BeginCategoryBrowse(struct SceneInventoryCategoryMenu *p)
{
    EnableWidget(&p->categoryWidget);
    EnableWidget(&p->primaryWidget);
    Storage_RefreshCategoryWidgets(p);
    Storage_RefreshSelectedItemDetails(p);
    SceneInventory_SetCategoryPalette(p, p->paletteTarget);
    SceneInventory_SetState(p, SceneInventory_UpdateCategoryBrowse, 0);
}
