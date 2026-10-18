#include "global.h"
extern u16 gUnk_03005260[];
s32 sub_0816B59C(void *, s32);
void sub_0816D920(void *, s32);
void sub_0816D868(void *);
void sub_08166B7C(void *, s32);
void Hud_SaveGridSelection(void *);
s32 Inventory_GetQuantityByStorage(s32, s32);
void Sound_PlaySong(s32);
void PauseMenu_AddSpriteFlags(void *, s32, s32);
void PauseMenu_OpenPrompt(u32, s32, s32);
void PauseMenu_SetConfirmationIndicator(void *, s32);
void sub_081663A0(void *, s32, s32, s32, s32);
void Hud_SetInteractionCallback(void *, void (*)(void), s32);
void sub_0817014C(void);
void sub_0816DAF8(void *);
void PauseMenu_OnItemUseSelectionUpdate(u8 *p)
{
    u8 *q = p + 0x4384;
    if (sub_0816B59C(q, 0xf0)) {
        sub_0816D920(p + 0x1620, p[0x4387]);
        sub_0816D868(p);
    }
    sub_08166B7C(p, 4);
    if (1 & gUnk_03005260[1]) {
        Hud_SaveGridSelection(q);
        if (Inventory_GetQuantityByStorage(p[0x4860], p[0x438a]) < 0)
            Sound_PlaySong(0x192);
        else {
            PauseMenu_AddSpriteFlags(p, 0x3b, 1);
            PauseMenu_OpenPrompt(*(u32 *)(p + 0x5c), 2, 1);
            PauseMenu_SetConfirmationIndicator(p, 0);
            sub_081663A0(p, 0, 0, 1, 0);
            Hud_SetInteractionCallback(p, sub_0817014C, 1);
        }
    } else if (2 & gUnk_03005260[1]) {
        Sound_PlaySong(0xde);
        sub_0816DAF8(p);
    }
}
