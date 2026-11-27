#include "global.h"
#include "ui/widgets/scene_input.h"
struct StorageCategoryMenu {
    u8 unknown0[0x258];
    struct AnimSprite primaryWidget;
    u8 unknown2B8[0x1200];
    struct AnimSprite categoryWidget;
    u8 unknown1518[0x2B0];
    u16 paletteTarget[16];
};
void Storage_RefreshCategoryWidgets(struct StorageCategoryMenu *);
void Storage_RefreshSelectedItemDetails(struct StorageCategoryMenu *);
void Storage_SetCategoryPalette(struct StorageCategoryMenu *, u16 *);
void Storage_UpdateCategoryBrowse(struct StorageCategoryMenu *);
void Storage_SetState(struct StorageCategoryMenu *,
                             void (*)(struct StorageCategoryMenu *), u32);
void Storage_BeginCategoryBrowse(struct StorageCategoryMenu *p)
{
    AnimSprite_Hide(&p->categoryWidget);
    AnimSprite_Hide(&p->primaryWidget);
    Storage_RefreshCategoryWidgets(p);
    Storage_RefreshSelectedItemDetails(p);
    Storage_SetCategoryPalette(p, p->paletteTarget);
    Storage_SetState(p, Storage_UpdateCategoryBrowse, 0);
}
