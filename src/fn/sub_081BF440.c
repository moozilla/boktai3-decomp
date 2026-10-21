
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
void Storage_InitSlotMapping(struct SceneInventoryCategoryMenu *, s32);
void Storage_RefreshCategoryWidgets(struct SceneInventoryCategoryMenu *);
void Storage_RefreshSelectedItemDetails(struct SceneInventoryCategoryMenu *);
void Storage_InitSelectionGrid(struct SceneInputGridView *, u32, u32, u32, u32);
void Storage_UpdateCursorPosition(struct SceneInputGridView *, void *);
void SceneInventory_BeginCategoryBrowse(struct SceneInventoryCategoryMenu *);
void Sound_PlaySong(u32);
void MenuPanel_ShowDescription(struct SceneInventoryCategoryMenu *, s32, const void *, void *);
void SceneInventory_ResetCategoryPalette(struct SceneInventoryCategoryMenu *, u16 *);
void SceneInventory_SetState(struct SceneInventoryCategoryMenu *,
                             void (*)(struct SceneInventoryCategoryMenu *), u32);
void SceneInventory_UpdateSortConfirmation(struct SceneInventoryCategoryMenu *);
void Storage_BeginCategoryMode(struct SceneInventoryCategoryMenu *p, u32 mode)
{
    int new_var;
    Storage_InitSlotMapping(p, (p->categoryKinds[p->category] & 1) << 4);
    Storage_RefreshCategoryWidgets(p);
    Storage_RefreshSelectedItemDetails(p);
    {
        u32 modeIndex = mode - 1;
        p->mode = mode;
        new_var = -1;
        if (modeIndex <= 1) {
            Storage_InitSelectionGrid(&p->grid, 0, 4, 0, 16);
            Storage_UpdateCursorPosition(&p->grid, p->cursor);
            SceneInventory_BeginCategoryBrowse(p);
        } else {
            Sound_PlaySong(0xDD);
            {
                s32 descriptionMode = 4;
                s32 none;
                void *description = p->description;
                void *panel = p;

                none = new_var;
                MenuPanel_ShowDescription(panel, descriptionMode, description, (void *)none);
            }
            SceneInput_BeginChoice(&p->prompt, p->choiceResource, &p->choiceState, 1, 0);
            SceneInventory_ResetCategoryPalette(p, p->colors);
            SceneInventory_SetState(p, SceneInventory_UpdateSortConfirmation, 0);
            Storage_RefreshCategoryWidgets(p);
        }
    }
}
