#include "global.h"
#include "scene_inventory_panel.h"
struct MenuPanel {
    u8 pad0[0x10A0];
    u32 bannerFlags;
    u8 pad10A4[0x1740 - 0x10A4];
    const void *text;
};
void MenuPanel_ShowDescription(struct MenuPanel *panel, s32 mode)
{
    ScrollingDialogue_ClearScript();
    panel->bannerFlags |= 1;
    MessageWindow_SetMessageTable(panel->text);
    MessageWindow_SetBounds(0, 16, 30, 2);
    MessageWindow_SetDisplayMode(1);
    MessageWindow_ShowMessage(mode);
}
