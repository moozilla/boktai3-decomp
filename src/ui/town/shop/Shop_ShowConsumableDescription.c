#include "ui/town/shop_panel.h"
void Shop_ShowConsumableDescription(struct ShopPanel *panel)
{
    s32 item;
    if (panel->selection <= 15) {
        item = *(panel->selection + gGameState->table160);
    } else if (panel->selection <= 23) {
        item = panel->catalog[panel->selection - 16 + panel->page * 8];
    } else
        item = -1;
    if (item < 0) {
        panel->display.banner.flags |= 1;
        MessageWindow_Clear();
        ScrollingDialogue_ClearScript();
    } else {
        panel->display.banner.flags &= ~1;
        Anim_LoadKeyframe(&panel->display.banner, &panel->display.gridResource, 394, 1);
        MessageWindow_SetMessageTable(panel->consumableText);
        MessageWindow_SetBounds(2, 16, 28, 2);
        ScrollingDialogue_SetNumberArgument(0, 999);
        ScrollingDialogue_SetNumberArgument(1, 999);
        MessageWindow_SetDisplayMode(1);
        MessageWindow_ShowMessage(item);
        {
            s32 text = Script_ParseStringRef(panel->consumableText) + 48;
            ScrollingDialogue_SetScript(Text_LookupString(text + item));
        }
    }
    SceneInputPanel_ShowItemComparison(panel->comparison[0], panel->comparison[1],
                                       &panel->display.widgetResource, 0, item);
    SceneInputPanel_ShowAttributeComparison(panel->comparison[2], panel->comparison[3],
                                            panel->comparison[4], &panel->display.widgetResource,
                                            item);
}
