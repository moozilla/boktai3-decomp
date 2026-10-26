#include "global.h"

/* tentative: per-frame input callback of the stat comparison screen */

#include "scene_hud_display.h"
#include "game_state.h"
struct StatTransferProfile {
    u8 data[16];
};
extern const struct StatTransferProfile gUnk_08E60F80[];
extern u16 gUnk_03005260[];
void MenuDisplay_UpdateActionPrompt(void *, int);
int SceneInput_NavigateGridKeys(void *, int);
void SceneHud_GetActionIconPosition(void *, int);
void PauseInventory_RefreshConsumableDescription(void *);
void Hud_SaveGridSelection(void *);
s32 Inventory_GetConsumableId(u32);
s32 Player_GetCandidateAttribute(s32, s32);
s32 Player_GetCurrentAttribute(s32);
void SceneHud_DrawComparisonBars(void *, s32, s32, s16 *);
void InventoryPanel_DrawStatComparison(void *, int, int, int, int, int);
s32 GameState_IsConsumableSlotEquipped(u32);
void GameState_UnequipConsumableSlot(u32);
void Player_RefreshEquipmentWithOptions(void *, const void *, s32);
void PlayerPanel_SnapshotValues(void *);
void sub_08166534(void *, s32, s32);
void sub_08170D78(void *);
void SceneHud_PrepareActionTransfer(void *, int, int);
void SceneHud_TickStatTransfer(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void sub_08171258(void *);
void PauseInventory_ReturnToConsumableActions(void *);
void SceneHud_UpdateStatComparison(struct SceneHudState *p)
{
    s16 differences[3];
    s16 *end;
    s32 zero = 0;
    s32 i;
    s32 none;
    void *input;
    u32 slot;
    end = &differences[2];
    do {
        *end-- = zero;
    } while ((s32)end >= (s32)differences);
    none = -1;
    MenuDisplay_UpdateActionPrompt(p, 3);
    input = p->actionInput;
    if (SceneInput_NavigateGridKeys(input, 240)) {
        SceneHud_GetActionIconPosition((u8 *)p + 0x1620, p->actionCursor);
        PauseInventory_RefreshConsumableDescription(p);
        Hud_SaveGridSelection(input);
        for (i = 0; i < 3; i++) {
            s32 value = Player_GetCandidateAttribute(i, Inventory_GetConsumableId(p->actionCursor));
            differences[i] = value - Player_GetCurrentAttribute(i);
        }
        SceneHud_DrawComparisonBars(p, 10, 6, differences);
        InventoryPanel_DrawStatComparison(p, 0, 0, 1, 0, differences[2]);
    } else if (gUnk_03005260[1] & 1) {
        slot = p->actionCursor;
        if (Inventory_GetConsumableId(slot) == none) {
            Sound_PlaySong(0x192);
        } else if (GameState_IsConsumableSlotEquipped(slot)) {
            GameState_UnequipConsumableSlot(slot);
            Sound_PlaySong(0xDD);
            Player_RefreshEquipmentWithOptions(
                p->actionOwner, &gUnk_08E60F80[p->comparisonProfiles[gGameState->party[0]]], none);
            PlayerPanel_SnapshotValues(p);
            sub_08166534(p, 10, 6);
            sub_08170D78(p);
            for (i = 0; i < 3; i++) {
                s32 value =
                    Player_GetCandidateAttribute(i, Inventory_GetConsumableId(p->actionCursor));
                differences[i] = value - Player_GetCurrentAttribute(i);
            }
            SceneHud_DrawComparisonBars(p, 10, 6, differences);
            InventoryPanel_DrawStatComparison(p, 0, 0, 1, 0, differences[2]);
        } else {
            Sound_PlaySong(0xDD);
            Hud_SaveGridSelection(input);
            SceneHud_PrepareActionTransfer(p, 0, p->actionSelection);
            Hud_SetInteractionCallback(p, SceneHud_TickStatTransfer, 1);
        }
    } else if (gUnk_03005260[1] & 2) {
        sub_08171258(p);
        Sound_PlaySong(0xDE);
        PauseInventory_ReturnToConsumableActions(p);
    }
}
