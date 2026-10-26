#include "global.h"
#include "game_state.h"
extern u8 *gUnk_02000580[];
void Equipment_AssignUniqueSlot(s32 slot, s32 id); /* sets slot */
void Hud_RefreshEquipmentDisplay(void);
void Player_SetActionRequest(u8 *actor, s32 id);

/* Marks id in the unlock mask, puts it in the first empty one of the four
   slots at +0x88 (-1 is empty), selects that slot, refreshes the HUD and,
   when the slot is still the selected one, equips it on the player actor
   (tentative reading). */
void GameState_AddEquipment(s32 id)
{
    s32 i;
    gGameState->unlockMask |= 1 << id;
    for (i = 0; i < 4; i++) {
        if (*(s16 *)(i * 2 + (u8 *)gGameState + 0x88) == -1) {
            Equipment_AssignUniqueSlot(i, id);
            gGameState->selected = i;
            Hud_RefreshEquipmentDisplay();
            if (i == gGameState->selected && gUnk_02000580[0])
                Player_SetActionRequest(gUnk_02000580[0], id);
            return;
        }
    }
}
