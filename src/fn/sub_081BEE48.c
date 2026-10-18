#include "global.h"
#include "scene_input.h"
#define SceneInventory_BeginCategoryBrowse sub_081BEE48
#define SceneInventory_SetCategoryPalette sub_081BE0AC
#define SceneInventory_SetState sub_081BDB34
#define SceneInventory_UpdateCategoryBrowse sub_081BF1AC
struct SceneInventoryCategoryMenu {
    u8 unknown0[0x258];
    struct SceneInputWidget primaryWidget;
    u8 unknown2B8[0x1200];
    struct SceneInputWidget categoryWidget;
    u8 unknown1518[0x2B0];
    u16 paletteTarget[16];
};
void sub_081BFAE0(struct SceneInventoryCategoryMenu *);
void Storage_RefreshSelectedItemDetails(struct SceneInventoryCategoryMenu *);
void sub_081BE0AC(struct SceneInventoryCategoryMenu *, u16 *);
void sub_081BF1AC(struct SceneInventoryCategoryMenu *);
void sub_081BDB34(struct SceneInventoryCategoryMenu *, void (*)(struct SceneInventoryCategoryMenu *), u32);
static inline void EnableWidget(struct SceneInputWidget *widget)
{
    widget->flags |= 1;
}
void sub_081BEE48(struct SceneInventoryCategoryMenu *p)
{
    EnableWidget(&p->categoryWidget);
    EnableWidget(&p->primaryWidget);
    sub_081BFAE0(p);
    Storage_RefreshSelectedItemDetails(p);
    sub_081BE0AC(p, p->paletteTarget);
    sub_081BDB34(p, sub_081BF1AC, 0);
}
