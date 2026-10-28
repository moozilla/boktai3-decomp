#include "global.h"
#include "scene_hud_display.h"
// MATCHING_CONCESSION: register-pinning

#define SceneHud_GetEquipmentSlotPosition SceneHud_GetActionIconPosition
int SceneInput_NavigateGridKeys(void *, int);
void SceneHud_GetActionIconPosition(void *, int);
void SceneHud_ShowEquipmentDetails(void *);
void MenuDisplay_UpdateActionPrompt(void *, int);
u32 PauseMenu_CanSwapCategoryEntries(void *, u32, u32, u32);
s32 Inventory_GetEquipmentId(u32);
void SceneHud_InitEquipmentTransfer(void *, s32, s32, s32);
void sub_0816676C(void *, int);
void Hud_UpdateInventorySwapAnimation(void);
void sub_08173D2C(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void Hud_RestoreGridSelection(void *);
void Hud_SetGridPosition(void *, int, int);
void PauseInventory_GetItemCursorPosition(void *, void *);
void SceneHud_TickEquipmentSwap(struct SceneHudState *p)
{
    void *q = p->actionInput;
    u16 one;
    u16 keys;
    u32 c;
    u32 two;
    if (SceneInput_NavigateGridKeys(q, 0xf0)) {
        SceneHud_GetActionIconPosition(&p->categoryWidgets[5].position, p->actionCursor);
        SceneHud_ShowEquipmentDetails(p);
        MenuDisplay_UpdateActionPrompt(p, 2);
    }
    keys = gPadState[1];
    one = 1;
    c = one;
    asm("" : "+r"(c));
    if (c & keys) {
        u8 *cursor = &p->actionCursor;
        u32 source = *cursor;
        u8 *selection = &p->actionSelection;
        if (!PauseMenu_CanSwapCategoryEntries(p, source, *selection, 2))
            Sound_PlaySong(0x192);
        else {
            Sound_PlaySong(0x38a);
            if (Inventory_GetEquipmentId(*selection) != 0xff)
                SceneHud_InitEquipmentTransfer(p, 0, *selection, *cursor);
            if (Inventory_GetEquipmentId(*cursor) != 0xff)
                SceneHud_InitEquipmentTransfer(p, 1, *cursor, *selection);
            p->categoryWidgets[*selection + 10].flags |= one;
            p->categoryWidgets[*cursor + 10].flags |= one;
            sub_0816676C(p, 2);
            Hud_SetInteractionCallback(p, Hud_UpdateInventorySwapAnimation, 1);
        }
    } else {
        two = 2;
        asm("" : "+r"(two));
        if (two & keys) {
            Hud_SetInteractionCallback(p, sub_08173D2C, 1);
            p->categoryWidgets[6].flags |= one;
            Hud_RestoreGridSelection(q);
            Hud_SetGridPosition(q, p->actionInput[1], p->actionInput[2]);
            PauseInventory_GetItemCursorPosition(&p->categoryWidgets[5].position, q);
            Sound_PlaySong(0xde);
        }
    }
}
