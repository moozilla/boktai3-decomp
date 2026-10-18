#include "global.h"
#include "scene_hud_display.h"
#include "scene_inventory_panel.h"
#define SceneHud_ShowActionResultText sub_0816F7CC
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void sub_081663A0(void *, int, int, int, int);
void sub_08166B7C(void *, int);
void sub_0816701C(void *, int, int, int, int, int);
void sub_081672FC(void *);
void Hud_SaveGridSelection(void *);
void sub_0816CDB8(void *, int, int);
void PauseMenu_ClearItemSlot(void *, u32);
void sub_0816D868(void *);
void sub_0816D920(void *, int);
void sub_0816D98C(void *);
void sub_0816FF4C(void);
void sub_0816F7CC(struct SceneHudState *s)
{
    u8 *input;
    u16 frame;
    if (!s->actionTransferDelay) {
        MessageWindow_SetMessageTable(s->actionTextScript);
        MessageWindow_SetDisplayMode(1);
        MessageWindow_SetBounds(0, 16, 30, 4);
        MessageWindow_ShowMessage(0x77);
    }
    frame = ++s->actionTransferDelay;
    if (frame > 89 || (frame > 15 && (gUnk_03005260[1] & 1))) {
        MessageWindow_SetBounds(0, 16, 30, 2);
        sub_0816D98C(s);
        Hud_SetInteractionCallback(s, sub_0816FF4C, 1);
        input = s->actionInput;
        sub_0816CDB8(input, 1, 0);
        sub_0816D920(&s->categoryWidgets[5].position, s->actionCursor);
        sub_0816D868(s);
        Hud_SaveGridSelection(input);
        sub_081672FC(s);
        sub_08166B7C(s, 4);
        sub_0816701C(s, 0, 0, 0, 1, 0);
        sub_081663A0(s, 1, 0, 0, 0);
    }
}
