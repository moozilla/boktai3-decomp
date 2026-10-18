#define SceneDialog_BeginTimedMessage sub_0817635C
#define SceneDialog_SetPhase Hud_SetInteractionCallback
#define SceneDialog_SelectMessage     sub_08175854
#define gSceneDialogActive            gUnk_03005408
#include "global.h"
struct SceneDialog {
    u8 pad0[0x5c];
    const u8 *script;
    u8 pad60[0xA4E - 0x60];
    u16 timer;
    u8 padA50[0x48C8 - 0xA50];
    s32 window;
};
extern u32 gUnk_03005408;
void TextWindow_SetBounds(s32, s32, s32, s32, s32);
s32 TextWindow_SetScript(s32, const u8 *);
s32 TextWindow_Refresh(s32, s32);
void TextWindow_StartPlayback(int);
void sub_08175854(u32);
void sub_081763D4(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void sub_0817635C(struct SceneDialog *p)
{
    gUnk_03005408 = 1;
    if (!p->timer) {
        TextWindow_SetBounds(p->window, 2, 6, 26, 6);
        TextWindow_SetScript(p->window, p->script);
        TextWindow_Refresh(p->window, 13);
        TextWindow_StartPlayback(p->window);
        sub_08175854(12);
    }
    if (++p->timer > 59)
        Hud_SetInteractionCallback(p, sub_081763D4, 1);
}
