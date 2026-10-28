#define SceneMenuPanel_SetCallback Hud_SetInteractionCallback
#include "global.h"
#include "input.h"
struct SceneMenuPanel {
    u8 unknown0[0x3C];
    const void *text;
    u8 unknown40[0xA0E];
    u16 delay;
    u8 unknownA50[0x3E11];
    u8 result;
};
s32 MessageWindow_SetMessageTable(const void *);
s32 MessageWindow_SetDisplayMode(s32);
s32 MessageWindow_SetBounds(s32, s32, s32, s32);
s32 MessageWindow_ShowMessage(s32);
s32 MessageWindow_Clear(void);
void SceneMenuPanel_SetCallback(void *, void (*)(void), s32);
void MenuTransition_UpdateSelectionFade(void);
void SceneMenuPanel_ShowTimedResult(struct SceneMenuPanel *panel)
{
    if (panel->delay == 16) {
        MessageWindow_SetMessageTable(panel->text);
        MessageWindow_SetDisplayMode(1);
        MessageWindow_SetBounds(0, 16, 30, 4);
        MessageWindow_ShowMessage(panel->result + 112);
    }
    ++panel->delay;
    if (panel->delay > 89 || (panel->delay > 15 && (gPadState[1] & 1))) {
        MessageWindow_Clear();
        SceneMenuPanel_SetCallback(panel, MenuTransition_UpdateSelectionFade, 1);
    }
}
