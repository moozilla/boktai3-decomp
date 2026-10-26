#include "scene_inventory_panel.h"
void SceneInventory_ShowItemDescription(struct SceneInventoryPanel *panel)
{
    s32 item;
    s32 frame;
    if (panel->selection <= 15) {
        item = Inventory_GetItemId(panel->selection);
        frame = SceneInventory_GetItemConditionFrame(panel->selection);
    } else if (panel->selection <= 23) {
        {
            s32 offset = panel->page * 8;
            offset -= 16;
            {
                u32 selected = panel->selection;
                offset += selected;
            }
            item = panel->catalog[offset];
        }
        if (item >= 0) {
            u32 condition = gUnk_08E61280[item].conditionKey;
            frame = 395;
            if (!condition)
                frame--;
        } else
            frame = 393;
    } else {
        item = -1;
        frame = 393;
    }
    if (item < 0) {
        panel->display.banner.flags |= 1;
        MessageWindow_Clear();
        ScrollingDialogue_ClearScript();
    } else {
        s32 text;
        panel->display.banner.flags &= ~1;
        Anim_LoadKeyframe(&panel->display.banner, &panel->display.gridResource, (u16)frame, 1);
        MessageWindow_SetMessageTable(panel->itemText);
        MessageWindow_SetBounds(2, 16, 28, 2);
        MessageWindow_SetDisplayMode(1);
        MessageWindow_ShowMessage(item);
        text = Script_ParseStringRef(panel->itemText);
        if (gGameState->form == 6 && item == 13) {
            ScrollingDialogue_SetScript(Text_LookupString(text + 234));
        } else {
            s32 context = gGameState->form;
            if (context == 0 || context == 6)
                text += 55;
            else
                text += 110;
            ScrollingDialogue_SetScript(Text_LookupString(text + item));
        }
    }
    SceneInputPanel_ShowItemComparison(panel->comparison[0], panel->comparison[1],
                                       &panel->display.widgetResource, 0, -1);
    SceneInputPanel_ShowAttributeComparison(panel->comparison[2], panel->comparison[3],
                                            panel->comparison[4], &panel->display.widgetResource,
                                            -1);
}
