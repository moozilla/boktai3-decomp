#include "global.h"
#include "game_state.h"
#include "player.h"

void Inventory_RemoveEquipmentEntry(s32);
void Player_BeginDescriptorAction(u8 *, s32);
void SceneHud_RefreshControls(void);
/* Applies the item in the selected slot (game state +0x50 indexes the table
   at +0x68) and refreshes the player and the HUD (tentative). */
void GameState_ApplySelectedItem(void)
{
    u8 *gs = gGameState;
    Inventory_RemoveEquipmentEntry(*(s16 *)(*(s16 *)(gs + 0x50) * 2 + gs + 0x68));
    if (gPlayers[0]) {
        Player_BeginDescriptorAction(gPlayers[0], 0);
        SceneHud_RefreshControls();
    }
}
