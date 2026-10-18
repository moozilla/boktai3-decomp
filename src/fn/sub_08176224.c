#define SelectionPanel_ConfirmTransition sub_08176224
#define SelectionPanel_ProcessFlaggedChoice sub_0817627C
#define MenuPanel_SetCallback Hud_SetInteractionCallback
#define gPanelInputState gUnk_03005260
// Provisional panel family; field meanings follow static consumers.
#include "global.h"
extern u16 gUnk_03005260[];
void Sound_PlaySong(u32);
void PauseMenu_SetConfirmationIndicator(void *, s32);
void sub_08175854(u32);
void PauseMenu_HidePromptIcons(void *);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void sub_081663A0(void *, int, int, int, int);
void sub_0817627C(void);
void sub_08176224(void *p)
{
    if (gUnk_03005260[1] & 1) {
        Sound_PlaySong(0xDD);
        PauseMenu_SetConfirmationIndicator(p, 1);
        sub_08175854(9);
        PauseMenu_HidePromptIcons(p);
        Hud_SetInteractionCallback(p, sub_0817627C, 1);
        sub_081663A0(p, 0, 0, 1, 0);
    }
}
