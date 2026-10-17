#include "global.h"

extern u8 *gUnk_02000710;
extern u8 *gUnk_02000580[];
void sub_0822D918(s32);
void sub_0813EABC(u8 *, s32);
void sub_081782E0(void);
/* Applies the item in the selected slot (game state +0x50 indexes the table
   at +0x68) and refreshes the player and the HUD (tentative). */
void GameState_ApplySelectedItem(void)
{
    u8 *gs = gUnk_02000710;
    sub_0822D918(*(s16 *)(*(s16 *)(gs + 0x50) * 2 + gs + 0x68));
    if (gUnk_02000580[0]) {
        sub_0813EABC(gUnk_02000580[0], 0);
        sub_081782E0();
    }
}
