#include "global.h"
#include "game_state.h"
extern u8 *gUnk_02000580[];
void sub_0822E458(s32 slot, s32 id); /* sets slot */
void sub_08178140(void);
void sub_0813F9EC(u8 *actor, s32 id);
#define GameState_AddEquipment sub_0822E498

/* Marks id in the unlock mask, puts it in the first empty one of the four
   slots at +0x88 (-1 is empty), selects that slot, refreshes the HUD and,
   when the slot is still the selected one, equips it on the player actor
   (tentative reading). */
void GameState_AddEquipment(s32 id)
{
    s32 i;
    gUnk_02000710->unlockMask |= 1 << id;
    for (i = 0; i < 4; i++) {
        if (*(s16 *)(i * 2 + (u8 *)gUnk_02000710 + 0x88) == -1) {
            sub_0822E458(i, id);
            gUnk_02000710->selected = i;
            sub_08178140();
            if (i == gUnk_02000710->selected && gUnk_02000580[0])
                sub_0813F9EC(gUnk_02000580[0], id);
            return;
        }
    }
}
