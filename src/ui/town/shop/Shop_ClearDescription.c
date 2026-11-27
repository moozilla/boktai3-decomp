#include "ui/town/shop_panel.h"
void Shop_ClearDescription(struct ShopPanel *p)
{
    p->display.banner.flags |= 1;
    MessageWindow_Clear();
    ScrollingDialogue_ClearScript();
    SceneInputPanel_ShowItemComparison(p->comparison[0], p->comparison[1],
                                       &p->display.widgetResource, 0, -1);
    SceneInputPanel_ShowAttributeComparison(p->comparison[2], p->comparison[3], p->comparison[4],
                                            &p->display.widgetResource, -1);
}
