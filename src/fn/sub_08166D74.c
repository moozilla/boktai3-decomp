#include "global.h"
#include "scene_item_values.h"
#include "game_state.h"
/* Tentative inventory display family, inferred from slot and item consumers. */
extern void *gUnk_02000580;
s32 GameState_GetUnlockEntry(s32);
void ActionRecord_CopyFromEntry(struct SceneValueEquipment *, void *);
void *Player_CalculatePreviewActionStat(void *, s32, void *, struct SceneValueEquipment *, s32);
s32 InventoryPanel_CreateEntry(void **out, s32 position, s32 index, s32 slotMode, s32 kind)
{
    s32 result;
    u8 data[0x20];
    struct SceneValueEquipment *equipment = 0;
    if (slotMode) {
        if (index > 7) {
            kind = 4;
            data[0] = GameState_GetUnlockEntry(
                          ((struct GameState *)((u8 *)gGameState + index * 2))->inventory[0]) -
                      1;
        } else {
            kind = 0;
            equipment = Inventory_GetEquipmentEntry(
                ((struct GameState *)((u8 *)gGameState + index * 2))->inventory[0]);
            ActionRecord_CopyFromEntry(equipment, data);
        }
    } else if (kind == 4) {
        data[0] = GameState_GetUnlockEntry(index) - 1;
    } else {
        kind = 0;
        equipment = Inventory_GetEquipmentEntry(index);
        ActionRecord_CopyFromEntry(equipment, data);
    }
    if (data[0] != 255) {
        *out = Player_CalculatePreviewActionStat(gUnk_02000580, position, data, equipment, kind);
        result = kind;
    } else {
        result = -1;
    }
    return result;
}
