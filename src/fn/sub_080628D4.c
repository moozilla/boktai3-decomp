#include "scene_inventory_tail.h"
#include "scene_inventory_panel.h"
#define SceneInventory_RefreshCatalogDisplay sub_080628D4
struct SceneInventoryPageView {
    u8 unknown0[0x1AB0];
    u8 page, pageCount;
};
void SceneInputPanel_SetMeterGlyph(struct SceneInputPanel *, u32, u32);
void SceneInventory_PopulateEquipmentGrid(struct SceneInventoryPanel *);
void SceneInventory_PopulateItemGrid(struct SceneInventoryPanel *);
void SceneInventory_PopulateConsumableGrid(struct SceneInventoryPanel *);
void SceneInventory_RefreshCatalogDisplay(struct SceneInventoryTail *p)
{
    struct SceneInventoryPanel *display = (struct SceneInventoryPanel *)p;
    struct SceneInventoryPageView *pages = (struct SceneInventoryPageView *)p;
    sub_08061540(p);
    SceneInputPanel_SetMeterGlyph(&p->display, pages->page, pages->pageCount);
    switch (p->category) {
        case 0:
            SceneInventory_PopulateEquipmentGrid(display);
            break;
        case 1:
            SceneInventory_PopulateItemGrid(display);
            break;
        case 2:
            SceneInventory_PopulateConsumableGrid(display);
            break;
    }
    SceneInputPanel_ShowItemComparison(display->comparison[0], display->comparison[1],
                                       &display->display.widgetResource, 0, -1);
    SceneInputPanel_ShowAttributeComparison(display->comparison[2], display->comparison[3],
                                            display->comparison[4],
                                            &display->display.widgetResource, -1);
}
