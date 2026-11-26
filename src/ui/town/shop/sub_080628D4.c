#include "ui/town/shop_tail.h"
#include "ui/town/shop_panel.h"
struct SceneInventoryPageView {
    u8 unknown0[0x1AB0];
    u8 page, pageCount;
};
void SceneInputPanel_SetMeterGlyph(struct SceneInputPanel *, u32, u32);
void Shop_PopulateEquipmentGrid(struct SceneInventoryPanel *);
void Shop_PopulateItemGrid(struct SceneInventoryPanel *);
void Shop_PopulateConsumableGrid(struct SceneInventoryPanel *);
void Shop_RefreshCatalogDisplay(struct SceneInventoryTail *p)
{
    struct SceneInventoryPanel *display = (struct SceneInventoryPanel *)p;
    struct SceneInventoryPageView *pages = (struct SceneInventoryPageView *)p;
    Shop_BeginBrowse(p);
    SceneInputPanel_SetMeterGlyph(&p->display, pages->page, pages->pageCount);
    switch (p->category) {
        case 0:
            Shop_PopulateEquipmentGrid(display);
            break;
        case 1:
            Shop_PopulateItemGrid(display);
            break;
        case 2:
            Shop_PopulateConsumableGrid(display);
            break;
    }
    SceneInputPanel_ShowItemComparison(display->comparison[0], display->comparison[1],
                                       &display->display.widgetResource, 0, -1);
    SceneInputPanel_ShowAttributeComparison(display->comparison[2], display->comparison[3],
                                            display->comparison[4],
                                            &display->display.widgetResource, -1);
}
