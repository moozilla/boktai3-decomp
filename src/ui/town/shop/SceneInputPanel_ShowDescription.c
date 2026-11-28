#include "ui/town/shop_panel.h"
#include "ui/widgets/scene_input_panel.h"
void SceneInputPanel_ShowDescription(void *owner, s32 mode, const void *unusedText, s32 unusedItem)
{
    struct ShopPanel *panel = owner;
    ScrollingDialogue_ClearScript();
    panel->display.banner.flags |= 1;
    MessageWindow_SetMessageTable((void *)panel->unknown1A88);
    MessageWindow_SetBounds(0, 16, 30, 2);
    MessageWindow_SetDisplayMode(1);
    MessageWindow_ShowMessage(mode);
}
