#include "global.h"
extern u16 gUnk_03005260[];
void sub_0816618C(void *, s32, s32);
void Sound_PlaySong(s32);
void Hud_SaveSecondaryGridSelection(void *);
void sub_0816D920(void *, s32);
void PauseInventory_BeginConsumableSortConfirmation(void *);
void sub_081712D4(void *, s32);
void PauseInventory_BeginConsumableSwapSelection(void *, s32);
void PauseInventory_BeginConsumableDiscardSelection(void *, s32);
void PauseMenu_ClearSpriteFlags(void *, s32, s32);
void PauseMenu_ResetItemCursor(void *);
s32 sub_0816B59C(void *, s32);
void PauseInventory_RefreshConsumableDescription(void *);
void sub_08166B7C(void *, s32);
void PauseInventory_UpdateConsumableActions(u8 *p)
{
    sub_0816618C(p, 8, 32);
    p[0xa58] = 0;
    if (1 & gUnk_03005260[1]) {
        u8 *q;
        Sound_PlaySong(0xdd);
        Hud_SaveSecondaryGridSelection(p + 0x4384);
        {
            u8 *a = p + 0x15c0;
            q = p + 0x4387;
            sub_0816D920(a, *q);
        }
        if (*q == 0x27) {
            PauseInventory_BeginConsumableSortConfirmation(p);
            return;
        }
        if (*q == 0x20)
            sub_081712D4(p, 1);
        else if (*q == 0x26)
            PauseInventory_BeginConsumableSwapSelection(p, 1);
        else if (*q == 0x28)
            PauseInventory_BeginConsumableDiscardSelection(p, 1);
        PauseMenu_ClearSpriteFlags(p, 5, 1);
        PauseMenu_ResetItemCursor(p);
        sub_0816D920(p + 0x1620, p[0x4387]);
    } else if (sub_0816B59C(p + 0x4384, 0xc0)) {
        sub_0816D920(p + 0x15c0, p[0x4387]);
        PauseInventory_RefreshConsumableDescription(p);
        sub_08166B7C(p, 3);
    }
}
