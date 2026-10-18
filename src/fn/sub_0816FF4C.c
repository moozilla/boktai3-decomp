#define SceneHud_UpdateActionSelection sub_0816FF4C
#define gSceneInputKeys                gUnk_03005260
#include "global.h"
#include "scene_hud_display.h"
extern u16 gUnk_03005260[];
void sub_08166B7C(void *, int);
int sub_0816B59C(void *, int);
void sub_0816D920(void *, int);
void sub_0816D868(void *);
void Hud_SaveGridSelection(void *);
s32 Inventory_GetQuantityByStorage(u32, u32);
int sub_0816D4E4(s32);
void PauseMenu_AddSpriteFlags(void *, int, int);
void PauseMenu_OpenPrompt(s32, s32, s32);
u32 PauseMenu_CanUseSelectedItem(void *);
void PauseMenu_SetConfirmationIndicator(void *, s32);
void sub_081663A0(void *, int, int, int, int);
void PauseMenu_HidePromptIcons(void *);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void PauseMenu_OnItemUseConfirmationUpdate(void);
void sub_0816DAF8(void *);
void sub_0816FF4C(struct SceneHudState *p)
{
    int result, index;
    u8 *input;
    sub_08166B7C(p, 4);
    input = p->actionInput;
    result = sub_0816B59C(input, 240);
    if (result) {
        sub_0816D920((u8 *)p + 0x1620, p->actionCursor);
        sub_0816D868(p);
        Hud_SaveGridSelection(input);
        sub_08166B7C(p, 4);
    } else if (gUnk_03005260[1] & 1) {
        index = Inventory_GetQuantityByStorage(p->actionCategory, p->actionSelection);
        if (sub_0816D4E4(index) && index != -1) {
            PauseMenu_AddSpriteFlags(p, 59, 1);
            PauseMenu_OpenPrompt(*(s32 *)((u8 *)p + 0x5c), 3, 1);
            PauseMenu_SetConfirmationIndicator(p, PauseMenu_CanUseSelectedItem(p));
            sub_081663A0(p, 0, 0, 1, result);
            PauseMenu_HidePromptIcons(p);
            Hud_SetInteractionCallback(p, PauseMenu_OnItemUseConfirmationUpdate, 1);
        } else
            Sound_PlaySong(0x192);
    } else if (gUnk_03005260[1] & 2) {
        sub_0816DAF8(p);
        Sound_PlaySong(222);
    }
}
