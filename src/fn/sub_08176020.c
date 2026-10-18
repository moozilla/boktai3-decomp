#define PauseMenu_BeginSleepPrompt sub_08176020
#define PauseMenu_SetCallback Hud_SetInteractionCallback
#define gPauseMenuKeys gUnk_03005260
#include "global.h"
struct PauseMenuSleep {
    u8 unknown0[0x5C];
    const u8 *script;
    u8 unknown60[0x4868];
    s32 promptWindow;
};
extern u16 gUnk_03005260[];
void Sound_PlaySong(u32);
void PauseMenu_SetConfirmationIndicator(struct PauseMenuSleep *, s32);
void sub_08175854(u32);
void PauseMenu_HidePromptIcons(struct PauseMenuSleep *);
void Hud_SetInteractionCallback(struct PauseMenuSleep *, void (*)(void), s32);
void sub_081760A4(void);
void sub_081663A0(struct PauseMenuSleep *, s32, s32, s32, s32);
void TextWindow_SetBounds(s32, s32, s32, s32, s32);
s32 TextWindow_SetScript(s32, const u8 *);
s32 TextWindow_Refresh(s32, s32);
void TextWindow_StartPlayback(s32);
void sub_08176020(struct PauseMenuSleep *s)
{
    s32 *window;
    if (gUnk_03005260[1] & 1) {
        Sound_PlaySong(0xDD);
        PauseMenu_SetConfirmationIndicator(s, 0);
        sub_08175854(0x12);
        PauseMenu_HidePromptIcons(s);
        Hud_SetInteractionCallback(s, sub_081760A4, 1);
        sub_081663A0(s, 0, 0, 1, 0);
        window = &s->promptWindow;
        TextWindow_SetBounds(*window, 4, 6, 0x16, 6);
        TextWindow_SetScript(*window, s->script);
        TextWindow_Refresh(*window, 0x10);
        TextWindow_StartPlayback(*window);
    }
}
