// MATCHING_CONCESSION: register-pinning
#include "scene_inventory_panel.h"
void SceneInventory_ShowConsumableDescription(struct SceneInventoryPanel *panel)
{
    s32 item;
    register const u8 *selectionPtr asm("r1") = &panel->selection;
    register u32 selection asm("r0") = *selectionPtr;
    if (selection <= 15) {
        struct GameState *state = gUnk_02000710;
        struct GameState *indexed;
        indexed = (void *)(selection * 2 + (u32)state);
        item = indexed->table160[0];
    } else if (selection <= 23) {
        s32 offset = panel->page * 8;
        offset -= 16;
        offset += *selectionPtr;
        item = panel->catalog[offset];
    } else
        item = -1;
    if (item < 0) {
        panel->display.banner.flags |= 1;
        sub_08030BF8();
        sub_08033468();
    } else {
        panel->display.banner.flags &= ~1;
        Anim_LoadKeyframe(&panel->display.banner, &panel->display.gridResource, 394, 1);
        sub_08030C84(panel->consumableText);
        sub_08030F20(2, 16, 28, 2);
        sub_08033478(0, 999);
        sub_08033478(1, 999);
        sub_08030B78(1);
        sub_08030DBC(item);
        {
            s32 text = Script_ParseStringRef(panel->consumableText);
            text += 48;
            text += item;
            sub_0803343C(Text_LookupString(text));
        }
    }
    SceneInputPanel_ShowItemComparison(panel->comparison[0], panel->comparison[1],
                                       &panel->display.widgetResource, 0, item);
    SceneInputPanel_ShowAttributeComparison(panel->comparison[2], panel->comparison[3],
                                            panel->comparison[4], &panel->display.widgetResource,
                                            item);
}
