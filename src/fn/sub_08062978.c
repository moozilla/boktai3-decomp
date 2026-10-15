#include "scene_inventory_panel.h"
void SceneInventory_ClearDescription(struct SceneInventoryPanel *p)
{
    p->display.banner.flags |= 1;
    sub_08030BF8();
    sub_08033468();
    SceneInputPanel_ShowItemComparison(p->comparison[0], p->comparison[1],
                                       &p->display.widgetResource, 0, -1);
    SceneInputPanel_ShowAttributeComparison(p->comparison[2], p->comparison[3], p->comparison[4],
                                            &p->display.widgetResource, -1);
}
