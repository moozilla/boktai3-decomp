#define Storage_BeginModeSelection sub_081BFC14
#define Storage_UpdateModeSelection sub_081BFA4C
#define Storage_RefreshCategoryWidgets sub_081BFAE0
#include "global.h"
#include "scene_input_navigation.h"
struct SceneInventoryCategoryMenu {
    u8 unknown0[0x258];
    struct SceneInputWidget prompt;
    u8 unknown2B8[0x1458 - 0x2B8];
    u8 cursor[0xC0];
    u8 choiceResource[0x1734 - 0x1518];
    const void *description;
    u8 unknown1738[0x15];
    u8 choiceState;
    u8 unknown174E[2];
    u32 mode;
    u8 category;
    u8 unknown1755[3];
    u8 categoryKinds[8];
    struct SceneInputGridView grid;
    u8 unknown1790[0x38];
    u16 colors[16];
};
void SceneInventory_SetCategoryPalette(struct SceneInventoryCategoryMenu *, u16 *);
void SceneInventory_SetState(struct SceneInventoryCategoryMenu *, void (*)(struct SceneInventoryCategoryMenu *), u32);
void Storage_HideItemDetails(struct SceneInventoryCategoryMenu *);
void Storage_InitSelectionGrid(struct SceneInputGridView *, u32, u32, u32, u32);
void Storage_InitSlotMapping(struct SceneInventoryCategoryMenu *, s32);
void Storage_UpdateCursorPosition(struct SceneInputGridView *, void *);
void sub_081BFAE0(struct SceneInventoryCategoryMenu *);
void sub_081BFA4C(struct SceneInventoryCategoryMenu *);
void sub_081BFC14(struct SceneInventoryCategoryMenu *p, s32 mode)
{
    Storage_InitSlotMapping(p, (p->categoryKinds[p->category] & 1) << 4);
    Storage_HideItemDetails(p);
    switch (mode) {
    case 1:
        Storage_InitSelectionGrid(&p->grid, 0, 8, 1, 3);
        break;
    case 3:
        Storage_InitSelectionGrid(&p->grid, 0, 8, 2, 3);
        break;
    case 2:
        Storage_InitSelectionGrid(&p->grid, 0, 8, 3, 3);
        break;
    }
    Storage_UpdateCursorPosition(&p->grid, p->cursor);
    SceneInventory_SetState(p, sub_081BFA4C, 1);
    SceneInventory_SetCategoryPalette(p, p->colors);
    sub_081BFAE0(p);
}
