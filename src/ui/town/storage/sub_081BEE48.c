#include "global.h"
#include "ui/widgets/scene_input.h"
struct SceneInventoryCategoryMenu {
    u8 unknown0[0x258];
    struct AnimSprite primaryWidget;
    u8 unknown2B8[0x1200];
    struct AnimSprite categoryWidget;
    u8 unknown1518[0x2B0];
    u16 paletteTarget[16];
};
void Storage_RefreshCategoryWidgets(struct SceneInventoryCategoryMenu *);
void Storage_RefreshSelectedItemDetails(struct SceneInventoryCategoryMenu *);
void Storage_SetCategoryPalette(struct SceneInventoryCategoryMenu *, u16 *);
void Storage_UpdateCategoryBrowse(struct SceneInventoryCategoryMenu *);
void Storage_SetState(struct SceneInventoryCategoryMenu *,
                             void (*)(struct SceneInventoryCategoryMenu *), u32);
void Storage_BeginCategoryBrowse(struct SceneInventoryCategoryMenu *p)
{
    AnimSprite_Hide(&p->categoryWidget);
    AnimSprite_Hide(&p->primaryWidget);
    Storage_RefreshCategoryWidgets(p);
    Storage_RefreshSelectedItemDetails(p);
    Storage_SetCategoryPalette(p, p->paletteTarget);
    Storage_SetState(p, Storage_UpdateCategoryBrowse, 0);
}
