#define MenuTransition_SetCallback Hud_SetInteractionCallback
#include "global.h"
#include "input.h"
void SceneHud_RefreshActionIcons(void *);
void SceneHud_UpdateActionSelection(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void sub_0816CDB8(void *, int, int);
void SceneHud_GetActionIconPosition(void *, int);
void SceneHud_ShowActionPreview(void *);
void Hud_SaveGridSelection(void *);
void InventoryPanel_ShowStatusIcons(void *);
void MenuDisplay_UpdateActionPrompt(void *, int);
void InventoryPanel_DrawStatComparison(void *, int, int, int, int, int);
void MenuDisplay_SetPaletteOverrides(void *, int, int, int, int);
s32 MessageWindow_SetDisplayMode(s32);
s32 MessageWindow_SetMessageTable(const void *);
s32 MessageWindow_ShowMessage(s32);
s32 MessageWindow_SetBounds(s32, s32, s32, s32);
struct MenuMessageView {
    u8 pad[0x3c];
    void *resource;
    u8 unknown40[0xa4e - 0x40];
    u16 timer;
};
void MenuTransition_FinishSelectionMessage(u8 *p)
{
    u16 *timer = &((struct MenuMessageView *)p)->timer;
    u8 *q, *a;
    u32 r;
    if (*timer == 0) {
        MessageWindow_SetMessageTable(((struct MenuMessageView *)p)->resource);
        MessageWindow_SetDisplayMode(1);
        MessageWindow_SetBounds(0, 16, 30, 4);
        MessageWindow_ShowMessage(p[0x4861] + 0x72);
    }
    {
        u32 next = *timer + 1;
        r = 0;
        *timer = next;
    }
    if (*timer > 89 || (*timer > 15 && (gPadState[1] & 1))) {
        *(u16 *)(p + 0x2ad6) = r;
        *(u32 *)(p + 0x2aa8) = (*(u32 *)(p + 0x2aa8) & ~2) | 1;
        MessageWindow_SetDisplayMode(1);
        MessageWindow_SetBounds(0, 16, 30, 2);
        SceneHud_RefreshActionIcons(p);
        Hud_SetInteractionCallback(p, SceneHud_UpdateActionSelection, 1);
        q = p + 0x4384;
        sub_0816CDB8(q, 1, 0);
        a = p + 0x1620;
        SceneHud_GetActionIconPosition(a, p[0x4387]);
        SceneHud_ShowActionPreview(p);
        Hud_SaveGridSelection(q);
        InventoryPanel_ShowStatusIcons(p);
        MenuDisplay_UpdateActionPrompt(p, 4);
        InventoryPanel_DrawStatComparison(p, 0, 0, 0, 1, r);
        MenuDisplay_SetPaletteOverrides(p, 1, 0, 0, r);
    }
}
