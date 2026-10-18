#include "global.h"
#include "player.h"
#include "game_state.h"
#include "scene_item_values.h"
void Player_BeginDescriptorAction(struct Player *, struct SceneValueEquipment *);
void Player_BeginIndexedAction(struct Player *, s32);
void Player_SetWeaponMode(struct Player *, s32);
s32 GameState_GetUnlockEntry(s32);
s32 SolarFruit_GetInventorySlot(s32);
void Player_RefreshActionEquipment(struct Player *p)
{
    if (p->actionMode == 6 || p->slot != 0)
        goto special;
    if ((s16)gUnk_02000710->unk50 <= 7) {
        s16 *entry = &((struct GameState *)((s16)gUnk_02000710->unk50 * 2 + (u32)gUnk_02000710))
                          ->inventory[0];
        if (*entry >= 0)
            Player_BeginDescriptorAction(p, Inventory_GetEquipmentEntry(*entry));
    } else {
        s16 *entry = &((struct GameState *)((s16)gUnk_02000710->unk50 * 2 + (u32)gUnk_02000710))
                          ->inventory[0];
        if (*entry >= 0) {
            Player_BeginIndexedAction(p, GameState_GetUnlockEntry(*entry));
            Player_SetWeaponMode(p,
                                 SolarFruit_GetInventorySlot(
                                     ((struct GameState *)((u8 *)gUnk_02000710 +
                                                           gUnk_02000710->actionAuxiliarySlot * 2))
                                         ->slots88[4]));
        } else
            Player_BeginDescriptorAction(p, 0);
    }
    return;
special:
    Player_BeginIndexedAction(p, 13);
}
