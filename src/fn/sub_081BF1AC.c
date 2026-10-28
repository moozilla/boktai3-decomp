#include "global.h"
#include "scene_input_navigation.h"
struct StorageBrowse {
    u8 unknown0[0x258];
    struct SceneInputWidget prompt;
    u8 unknown2B8[0x11A0];
    struct SceneInputWidget cursor, savedCursor;
    u8 resource[0x21C];
    void *description;
    u8 unknown1738[0x15];
    u8 choiceState;
    u8 unknown174E[2];
    u32 mode;
    u8 category;
    u8 unknown1755[3];
    u8 categoryKinds[8];
    struct SceneInputGridView grid;
    u8 slots[32];
    u8 unknown17B0[0x14];
    u8 choiceRepeat;
    u8 unknown17C5[3];
    u16 colors[16];
};
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
void Sound_PlaySong(u32);
void Storage_SetGridFlags(void *, u32);
void SceneInventory_SetState(void *, void (*)(void *), u32);
void Storage_UpdateSwapSelection(void *);
void Storage_RefreshCategoryWidgets(void *);
void Storage_BeginModeSelection(void *, u32);
s32 Inventory_GetItemId(u32);
s32 Inventory_GetEquipmentId(u32);
s32 Inventory_CanAssignEquipment(u32);
s32 GameState_IsConsumableSlotEquipped(u32);
void MenuPanel_ShowDescription(void *, s32, void *, s32);
void SceneInventory_ResetCategoryPalette(void *, void *);
void SceneInventory_UpdateCategoryConfirmation(void *);
void SceneInventory_BeginCategoryBrowse(void *);
void Storage_UpdateCursorPosition(void *, void *);
void Storage_RefreshSelectedItemDetails(void *);
void SceneInventory_UpdateCategoryBrowse(struct StorageBrowse *p)
{
    if (gPadState[0] & 4) {
        Anim_LoadKeyframe(&p->cursor, p->resource, 0x4D, 1);
        if (gPadState[1] & 1) {
            Sound_PlaySong(0xDC);
            if (++p->categoryKinds[6] > 2)
                p->categoryKinds[6] = 0;
        }
    } else if (gPadState[1] & 1) {
        SceneInput_SaveSelection(&p->grid);
        if (p->mode == 1) {
            struct SceneInputWidget *saved = &p->savedCursor;
            saved->flags &= ~1;
            p->savedCursor.position = p->cursor.position;
            Storage_SetGridFlags(&p->grid, 0x10);
            Sound_PlaySong(0xDD);
            SceneInventory_SetState(p, Storage_UpdateSwapSelection, 0);
            Storage_RefreshCategoryWidgets(p);
        } else if (p->mode == 2) {
            s32 slot = p->slots[p->grid.savedSelection];
            u32 allowed;
            switch (p->categoryKinds[p->category] >> 1) {
                case 0:
                    if (Inventory_GetItemId(slot) >= 0)
                        goto allow;
                    goto error;
                case 1:
                    if (Inventory_GetEquipmentId(slot) == 0xFF)
                        goto error;
                    if (Inventory_CanAssignEquipment(slot))
                        goto allow;
                    goto error;
                case 2:
                    if (*(s16 *)((u8 *)gGameState + 0x160 + slot * 2) < 0)
                        goto error;
                    if (GameState_IsConsumableSlotEquipped(slot)) {
                        allowed = 0;
                        goto check;
                    }
                    break;
            }
        allow:
            allowed = 1;
        check:
            if (allowed) {
                Sound_PlaySong(0xDD);
                MenuPanel_ShowDescription(p, 2, p->description, -1);
                SceneInput_BeginChoice(&p->prompt, p->resource, &p->choiceState, 0,
                                       &p->choiceRepeat);
                SceneInventory_ResetCategoryPalette(p, p->colors);
                SceneInventory_SetState(p, SceneInventory_UpdateCategoryConfirmation, 0);
            } else {
            error:
                Sound_PlaySong(0x192);
                SceneInventory_BeginCategoryBrowse(p);
            }
        }
    } else if (gPadState[1] & 2) {
        Sound_PlaySong(0xDE);
        Storage_BeginModeSelection(p, p->mode);
    } else if (SceneInput_NavigateGrid(&p->grid)) {
        Storage_UpdateCursorPosition(&p->grid, &p->cursor);
        Storage_RefreshSelectedItemDetails(p);
    }
}
