#include "scene_input_panel.h"
struct SceneInputPanelActionView {
    u8 unknown0[0x1AEE];
    u16 frame;
    u8 unknown1AF0[0x1CF8 - 0x1AF0];
    u32 action;
};
void SceneInputPanel_RequestAction(struct SceneInputPanelActionView *panel, u32 action)
{
    panel->action = action;
    panel->frame = 0;
}
