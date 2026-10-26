#include "global.h"
#include "scene_inventory_panel.h"
struct StoragePanel {
    u8 unknown0[0x1098];
    struct SceneInputWidget banner;
    u8 unknown10F8[0x1538 - 0x10F8];
    struct SceneInputResource gridResource;
    u8 unknown1558[0x1734 - 0x1558];
    const void *itemText;
    u8 unknown1738[0x29];
    u8 selection;
    u8 unknown1762;
    u8 savedSelection;
    u8 unknown1764[0x2c];
    u8 items[1];
};
void Storage_ShowItemDescription(struct StoragePanel *panel)
{
    s32 item, frame;
    if (panel->selection <= 7) {
        item = Inventory_GetItemId(panel->items[panel->savedSelection]);
        frame = SceneInventory_GetItemConditionFrame(panel->items[panel->savedSelection]);
    } else {
        item = -1;
        frame = 0xCF;
    }
    if (item < 0) {
        panel->banner.flags |= 1;
        MessageWindow_Clear();
        ScrollingDialogue_ClearScript();
    } else {
        s32 text, context;
        panel->banner.flags &= ~1;
        Anim_LoadKeyframe(&panel->banner, &panel->gridResource, (u16)frame, 1);
        MessageWindow_SetMessageTable(panel->itemText);
        MessageWindow_SetBounds(2, 16, 28, 2);
        MessageWindow_SetDisplayMode(1);
        MessageWindow_ShowMessage(item);
        Script_ParseStringRef(panel->itemText);
        text = Script_ParseStringRef(panel->itemText);
        context = gGameState->form;
        if (context == 0 || context == 6)
            text += 55;
        else
            text += 110;
        ScrollingDialogue_SetScript(Text_LookupString(text + item));
    }
}
