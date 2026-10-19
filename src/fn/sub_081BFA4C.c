#define Storage_UpdateModeSelection sub_081BFA4C
#define Storage_BeginCategoryMode sub_081BF440
#define Storage_RefreshCategoryWidgets sub_081BFAE0
#define gStorageInputKeys gUnk_03005260
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
void sub_081BF440(struct SceneInventoryCategoryMenu *, u32);
u32 SceneInput_NavigateGrid(struct SceneInputGridView *);
void sub_081BFAE0(struct SceneInventoryCategoryMenu *);
void Storage_UpdateCursorPosition(struct SceneInputGridView *, void *);
void sub_081BFA4C(struct SceneInventoryCategoryMenu *p)
{
    Storage_SetInteractionMode(p, 1);
    if (gUnk_03005260[1] & 1) {
        Sound_PlaySong(0xDD);
        SceneInventory_SetCategoryPalette(p, p->colors);
        switch (p->grid.selection) {
        case 0x22:
            sub_081BF440(p, 1);
            break;
        case 0x21:
            sub_081BF440(p, 3);
            break;
        case 0x20:
            sub_081BF440(p, 2);
            break;
        }
    } else {
        if (SceneInput_NavigateGrid(&p->grid)) {
            sub_081BFAE0(p);
            Storage_UpdateCursorPosition(&p->grid, p->cursor);
        }
    }
}
