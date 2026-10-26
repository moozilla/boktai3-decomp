#include "global.h"
#include "scene_inventory_panel.h"
struct StoragePanel {
    u8 unknown0[0x1098];
    struct SceneInputWidget banner;
    u8 unknown10F8[0x1538 - 0x10F8];
    struct SceneInputResource gridResource;
    u8 unknown1558[0x173C - 0x1558];
    const void *consumableText;
    u8 unknown1740[0x21];
    u8 selection;
    u8 unknown1762;
    u8 savedSelection;
    u8 unknown1764[0x2c];
    u8 items[1];
};
void Storage_ShowConsumableDescription(struct StoragePanel *panel)
{
    s32 item;
    if (panel->selection <= 7) {
        u32 selection = panel->items[panel->savedSelection];
        struct GameState *state = gGameState;
        struct GameState *indexed = (void *)(selection * 2 + (u32)state);
        item = indexed->table160[0];
    } else
        item = -1;
    if (item < 0) {
        panel->banner.flags |= 1;
        MessageWindow_Clear();
        ScrollingDialogue_ClearScript();
    } else {
        panel->banner.flags &= ~1;
        Anim_LoadKeyframe(&panel->banner, &panel->gridResource, 0xCF, 1);
        MessageWindow_SetMessageTable(panel->consumableText);
        MessageWindow_SetBounds(2, 16, 28, 2);
        ScrollingDialogue_SetNumberArgument(0, 999);
        ScrollingDialogue_SetNumberArgument(1, 999);
        MessageWindow_SetDisplayMode(1);
        MessageWindow_ShowMessage(item);
        {
            s32 text = Script_ParseStringRef(panel->consumableText);
            text += 48;
            text += item;
            ScrollingDialogue_SetScript(Text_LookupString(text));
        }
    }
}
