#include "ui/town/shop_tail.h"
#include "ui/town/shop_panel.h"
struct ShopPageView {
    u8 unknown0[0x1AB0];
    u8 page, pageCount;
};
void SceneInputPanel_SetMeterGlyph(struct SceneInputPanel *, u32, u32);
void Shop_PopulateEquipmentGrid(struct ShopPanel *);
void Shop_PopulateItemGrid(struct ShopPanel *);
void Shop_PopulateConsumableGrid(struct ShopPanel *);
void Shop_RefreshCatalogDisplay(struct ShopTail *p)
{
    struct ShopPanel *display = (struct ShopPanel *)p;
    struct ShopPageView *pages = (struct ShopPageView *)p;
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
