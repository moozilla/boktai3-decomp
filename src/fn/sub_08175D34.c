#define SelectionPanel_ConfirmSettings sub_08175D34
#define MenuPanel_SetCallback Hud_SetInteractionCallback
#define gPanelInputState gUnk_03005260
// Provisional panel family; field meanings follow static consumers.
#include "global.h"
extern u16 gUnk_03005260[];
void Sound_PlaySong(u32);
void sub_0817597C(void *, int);
void sub_08175854(u32);
void PauseMenu_HidePromptIcons(void *);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void sub_08175D78(void);
void sub_08175D34(void *p)
{
    if (gUnk_03005260[1] & 1) {
        Sound_PlaySong(0xDD);
        sub_0817597C(p, 1);
        sub_08175854(6);
        PauseMenu_HidePromptIcons(p);
        Hud_SetInteractionCallback(p, sub_08175D78, 1);
    }
}
