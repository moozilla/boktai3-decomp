#include "global.h"
void PauseMenu_CloseTextWindows(u8 *);
void MessageWindow_Clear(void);
void ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_HideImmediately(void);
void MessageWindow_SetBounds(u32, u32, u32, u32);
extern u8 *gUnk_02000710;
extern u32 gUnk_08611BA8[], gUnk_08611BC0[], gUnk_08611BD8[];
void PauseMenu_HideAllSprites(u8 *);
void Anim_LoadKeyframe(u8 *, u8 *, u32, u32);
void PauseMenu_SetPromptIconPositions(u8 *, u32, u32);
void sub_081660D0(u8 *, u32, s32);
void Hud_SetupBackgroundLayer(u8 *, u32, u32, u32, u32);
void sub_0816C838(u8 *);
void sub_081663A0(u8 *, u32, u32, u32, u32);
void MapScreen_InitAreaView(u8 *s)
{
    s16 *index;
    u8 *obj;
    PauseMenu_CloseTextWindows(s);
    MessageWindow_Clear();
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_HideImmediately();
    MessageWindow_SetBounds(0, 0x12, 0x1E, 2);
    *(u32 *)(s + 0x48F8) = gUnk_08611BA8[*(index = (s16 *)(gUnk_02000710 + 0x5AC))];
    *(u32 *)(s + 0x48FC) = gUnk_08611BC0[*index];
    *(u32 *)(s + 0x4900) = gUnk_08611BD8[*index];
    PauseMenu_HideAllSprites(s);
    obj = s + 0x1420;
    *(u32 *)(obj + 8) &= ~1;
    Anim_LoadKeyframe(obj, s + 0x64, 0x51, 0);
    PauseMenu_SetPromptIconPositions(s, 0, 0x18);
    sub_081660D0(s, 1, -1);
    Hud_SetupBackgroundLayer(s, *(u32 *)(s + 0x18), 0, 5, *(u32 *)(s + 0x30));
    sub_0816C838(s);
    sub_081663A0(s, 0, 0, 0, 0);
}
