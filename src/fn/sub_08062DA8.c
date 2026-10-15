#include "scene_inventory_catalog.h"
#include "scene_inventory_panel.h"
#define SceneInventory_InitMessageBox sub_08062DA8
void sub_08033568(void);
void sub_080311B0(s32);
void SceneInventory_InitMessageBox(struct SceneInventoryPanel *p)
{
    if (Script_SeekToKeyword(0x73))
        p->unknown1A88 = sub_08227E90();
    sub_08033568();
    sub_08030F20(0, 16, 30, 2);
    sub_08030B78(1);
    sub_080311B0(0x81B);
}
