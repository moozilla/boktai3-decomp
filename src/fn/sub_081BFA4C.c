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
void Storage_SetInteractionMode(struct SceneInventoryCategoryMenu *, u8);
void Sound_PlaySong(u32);
void SceneInventory_SetCategoryPalette(struct SceneInventoryCategoryMenu *, u16 *);
void Storage_BeginCategoryMode(struct SceneInventoryCategoryMenu *, u32);
u32 SceneInput_NavigateGrid(struct SceneInputGridView *);
void Storage_RefreshCategoryWidgets(struct SceneInventoryCategoryMenu *);
void Storage_UpdateCursorPosition(struct SceneInputGridView *, void *);
void Storage_UpdateModeSelection(struct SceneInventoryCategoryMenu *p)
{
    Storage_SetInteractionMode(p, 1);
    if (gPadState[1] & 1) {
        Sound_PlaySong(0xDD);
        SceneInventory_SetCategoryPalette(p, p->colors);
        switch (p->grid.selection) {
            case 0x22:
                Storage_BeginCategoryMode(p, 1);
                break;
            case 0x21:
                Storage_BeginCategoryMode(p, 3);
                break;
            case 0x20:
                Storage_BeginCategoryMode(p, 2);
                break;
        }
    } else {
        if (SceneInput_NavigateGrid(&p->grid)) {
            Storage_RefreshCategoryWidgets(p);
            Storage_UpdateCursorPosition(&p->grid, p->cursor);
        }
    }
}
