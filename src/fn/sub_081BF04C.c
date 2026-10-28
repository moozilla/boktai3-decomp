#include "global.h"
#include "scene_input_navigation.h"
struct MenuPanel {
    u8 unknown0[0x1458];
    u8 cursor[0xC0];
    u8 cursorResource[0x238];
    u32 mode;
    u8 unknown1754[0xA];
    u8 clockMode, unknown175F;
    struct SceneInputGridView grid;
};
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
s32 MenuPanel_FindEmptyOppositeHalf(struct MenuPanel *, u32);
s32 MenuPanel_IsEntryAvailable(struct MenuPanel *, s32);
s32 MenuPanel_IsEntryEmpty(struct MenuPanel *, s32);
u32 SceneInput_NavigateGrid(struct SceneInputGridView *);
void SceneInventory_BeginCategoryBrowse(struct MenuPanel *);
void SceneInventory_SetState(struct MenuPanel *, void (*)(struct MenuPanel *), u32);
void Sound_PlaySong(u32);
void Storage_RefreshSelectedItemDetails(struct MenuPanel *);
void Storage_UpdateCursorPosition(struct SceneInputGridView *, void *);
void Storage_BeginSwapTransition(struct MenuPanel *);
void Storage_UpdateItemTransition(struct MenuPanel *);
static inline u8 distinctInventorySlots(s32 source, s32 destination)
{
    u32 result = 0;
    if (source <= 15 && destination <= 15)
        result = (u32)source != (u32)destination;
    return result;
}
void Storage_UpdateSwapSelection(struct MenuPanel *p)
{
    if (gPadState[0] & 4) {
        Anim_LoadKeyframe(p->cursor, p->cursorResource, 0x4D, 1);
        if (gPadState[1] & 1) {
            Sound_PlaySong(0xDC);
            if (++p->clockMode > 2)
                p->clockMode = 0;
        }
    } else if (gPadState[1] & 1) {
        if (p->mode == 1) {
            s32 source, destination, empty;
            if (p->grid.selection == p->grid.savedSelection &&
                !MenuPanel_IsEntryEmpty(p, p->grid.selection)) {
                empty = MenuPanel_FindEmptyOppositeHalf(p, p->grid.selection);
                if (empty >= 0)
                    p->grid.savedSelection = empty;
            }
            {
                u8 *sourceSlot = &p->grid.selection;
                u8 *destinationSlot = &p->grid.savedSelection;
                source = *sourceSlot;
                destination = *destinationSlot;
            }
            if (source != destination &&
                (!MenuPanel_IsEntryEmpty(p, source) || !MenuPanel_IsEntryEmpty(p, destination))) {
                if ((!MenuPanel_IsEntryAvailable(p, source) &&
                     !MenuPanel_IsEntryAvailable(p, destination)) ||
                    distinctInventorySlots(source, destination)) {
                    Storage_BeginSwapTransition(p);
                    SceneInventory_SetState(p, Storage_UpdateItemTransition, 0);
                    return;
                }
            }
        }
        Sound_PlaySong(0x192);
    } else if (gPadState[1] & 2) {
        Sound_PlaySong(0xDE);
        SceneInventory_BeginCategoryBrowse(p);
    } else {
        if (SceneInput_NavigateGrid(&p->grid)) {
            Storage_UpdateCursorPosition(&p->grid, p->cursor);
            Storage_RefreshSelectedItemDetails(p);
        }
    }
}
