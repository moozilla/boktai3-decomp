#include "scene_input_panel.h"
struct SceneInputPanelMessageView {
    u8 unknown0[0xD08];
    struct SceneInputWidget banner;
    u8 unknownD68[0x1A88 - 0xD68];
    void *text;
};
u32 ScrollingDialogue_ClearScript(void);
s32 MessageWindow_SetMessageTable(void *);
void MessageWindow_SetBounds(s32, s32, s32, s32);
void MessageWindow_SetDisplayMode(s32);
void MessageWindow_ShowMessage(s32);
void SceneInputPanel_ShowDescription(void *owner, s32 mode, const void *unusedText, s32 unusedItem)
{
    struct SceneInputPanelMessageView *panel = owner;
    ScrollingDialogue_ClearScript();
    panel->banner.flags |= 1;
    MessageWindow_SetMessageTable(panel->text);
    MessageWindow_SetBounds(0, 16, 30, 2);
    MessageWindow_SetDisplayMode(1);
    MessageWindow_ShowMessage(mode);
}
