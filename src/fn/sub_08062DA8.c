#include "scene_inventory_catalog.h"
#include "scene_inventory_panel.h"
void ScrollingDialogue_RequestShow(void);
void MessageWindow_SetPalette(s32);
void SceneInventory_InitMessageBox(struct SceneInventoryPanel *p)
{
    if (Script_SeekToKeyword(0x73))
        p->unknown1A88 = Script_GetTextSourcePc();
    ScrollingDialogue_RequestShow();
    MessageWindow_SetBounds(0, 16, 30, 2);
    MessageWindow_SetDisplayMode(1);
    MessageWindow_SetPalette(0x81B);
}
