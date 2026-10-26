#include "global.h"
#include "game_state.h"
s32 InventoryPanel_CreateEntry(s32 *, s32, s32, s32, s32);
s32 InventoryPanel_UpdateSelectedUnlock(void *, s32, s32, s32, s32);
s32 InventoryPanel_UpdateSelectedEquipment(void *, s32, s32, s32, s32);
void InventoryPanel_SetEntryRow(void *, s32, s32, s32, s32, s32);
void SceneHud_FillDigitCells(s32, s32, s32, s32);
static inline void DrawComparison(void *p, s32 x, s32 y, s32 value, s32 delta, s32 old)
{
    if (delta > 0)
        InventoryPanel_SetEntryRow(p, x, y, value, 10, 0x47);
    else if (delta < 0)
        InventoryPanel_SetEntryRow(p, x, y, value, 20, 0x47);
    else
        InventoryPanel_SetEntryRow(p, x, y, old, 0, 0x47);
}
void InventoryPanel_DrawStatComparison(u8 *p, s32 unlock, s32 equipment, s32 selected, s32 active,
                                       s32 position)
{
    s32 value, old, delta, item, kind;
    if (active) {
        if ((s16)gGameState->unk50 <= 7) {
            item =
                ((struct GameState *)((s16)gGameState->unk50 * 2 + (u32)gGameState))->inventory[0];
            kind = 0;
        } else {
            item =
                ((struct GameState *)((s16)gGameState->unk50 * 2 + (u32)gGameState))->inventory[0];
            kind = 4;
        }
        if (InventoryPanel_CreateEntry(&value, position, item, 0, kind) < 0) {
            InventoryPanel_UpdateSelectedUnlock(p, 8, 3, 0, 0);
            InventoryPanel_UpdateSelectedEquipment(p, 8, 4, 0, 0);
        } else if (kind == 0) {
            old = InventoryPanel_UpdateSelectedEquipment(p, 0, 0, 1, 0);
            delta = value - old;
            SceneHud_FillDigitCells(3, 8, 3, 0);
            DrawComparison(p, 8, 4, value, delta, old);
        } else {
            old = InventoryPanel_UpdateSelectedUnlock(p, 0, 0, 1, 0);
            delta = value - old;
            SceneHud_FillDigitCells(3, 8, 4, 0);
            DrawComparison(p, 8, 3, value, delta, old);
        }
    } else if (selected) {
        if ((s16)gGameState->unk50 <= 7) {
            item =
                ((struct GameState *)((s16)gGameState->unk50 * 2 + (u32)gGameState))->inventory[0];
            kind = 0;
        } else {
            item =
                ((struct GameState *)((s16)gGameState->unk50 * 2 + (u32)gGameState))->inventory[0];
            kind = 4;
        }
        if (InventoryPanel_CreateEntry(&value, position, item, 0, kind) < 0) {
            InventoryPanel_UpdateSelectedUnlock(p, 8, 3, 0, 0);
            InventoryPanel_UpdateSelectedEquipment(p, 8, 4, 0, 0);
        } else if (kind == 0) {
            old = InventoryPanel_UpdateSelectedEquipment(p, 0, 0, 1, 0);
            delta = value - old;
            SceneHud_FillDigitCells(3, 8, 3, 0);
            DrawComparison(p, 8, 4, value, delta, old);
        } else {
            old = InventoryPanel_UpdateSelectedUnlock(p, 0, 0, 1, 0);
            delta = value - old;
            SceneHud_FillDigitCells(3, 8, 4, 0);
            DrawComparison(p, 8, 3, value, delta, old);
        }
    } else if (equipment) {
        if (InventoryPanel_CreateEntry(&value, position, p[0x4387], 0, 0) < 0) {
            InventoryPanel_UpdateSelectedUnlock(p, 8, 3, 0, 0);
            InventoryPanel_UpdateSelectedEquipment(p, 8, 4, 0, 0);
        } else {
            old = InventoryPanel_UpdateSelectedEquipment(p, 0, 0, 1, 0);
            delta = value - old;
            SceneHud_FillDigitCells(3, 8, 3, 0);
            DrawComparison(p, 8, 4, value, delta, old);
        }
    } else if (unlock) {
        if (InventoryPanel_CreateEntry(&value, 0, p[0x4387] - 8, 0, 4) < 0)
            InventoryPanel_UpdateSelectedUnlock(p, 3, 14, 0, 0);
        else {
            old = InventoryPanel_UpdateSelectedUnlock(p, 0, 0, 1, 0);
            delta = value - old;
            DrawComparison(p, 3, 14, value, delta, old);
        }
    }
}
