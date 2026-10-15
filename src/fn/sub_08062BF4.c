#include "scene_inventory_catalog.h"
#include "scene_inventory_tail.h"
void SceneInventory_InitCatalogBackground(struct SceneInventoryTail *p, s32 mode)
{
    SceneInputPanel_InitBackground(&p->display, mode,
                                   ((struct SceneInventoryScriptCatalog *)p)->pageCount);
}
