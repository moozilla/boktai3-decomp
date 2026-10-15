#include "scene_input_panel.h"
struct SceneInputPanelMessageView {
    u8 unknown0[0xD08];
    struct SceneInputWidget banner;
    u8 unknownD68[0x1A88 - 0xD68];
    void *text;
};
u32 sub_08033468(void);
s32 sub_08030C84(void *);
void sub_08030F20(s32, s32, s32, s32);
void sub_08030B78(s32);
void sub_08030DBC(s32);
void SceneInputPanel_ShowDescription(struct SceneInputPanelMessageView *panel, s32 mode)
{
    sub_08033468();
    panel->banner.flags |= 1;
    sub_08030C84(panel->text);
    sub_08030F20(0, 16, 30, 2);
    sub_08030B78(1);
    sub_08030DBC(mode);
}
