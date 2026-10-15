// MATCHING_CONCESSION: register-pinning
#include "scene_inventory_panel.h"
void SceneInventory_ShowItemDescription(struct SceneInventoryPanel *panel)
{
    s32 item, frame;
    if (panel->selection <= 15) {
        item = sub_0822CDA8(panel->selection);
        frame = sub_0816D820(panel->selection);
    } else if (panel->selection <= 23) {
        {
            register s32 offset asm("r0") = panel->page * 8;
            offset -= 16;
            {
                register u32 selected asm("r4") = panel->selection;
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
        sub_08030BF8();
        sub_08033468();
    } else {
        s32 text;
        panel->display.banner.flags &= ~1;
        Anim_LoadKeyframe(&panel->display.banner, &panel->display.gridResource, (u16)frame, 1);
        sub_08030C84(panel->itemText);
        sub_08030F20(2, 16, 28, 2);
        sub_08030B78(1);
        sub_08030DBC(item);
        text = Script_ParseStringRef(panel->itemText);
        if (gUnk_02000710->form == 6 && item == 13) {
            sub_0803343C(Text_LookupString(text + 234));
        } else {
            s32 context = gUnk_02000710->form;
            if (context == 0 || context == 6)
                text += 55;
            else
                text += 110;
            sub_0803343C(Text_LookupString(text + item));
        }
    }
    SceneInputPanel_ShowItemComparison(panel->comparison[0], panel->comparison[1],
                                       &panel->display.widgetResource, 0, -1);
    SceneInputPanel_ShowAttributeComparison(panel->comparison[2], panel->comparison[3],
                                            panel->comparison[4], &panel->display.widgetResource,
                                            -1);
}
