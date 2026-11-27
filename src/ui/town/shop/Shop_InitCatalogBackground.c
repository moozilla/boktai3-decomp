#include "ui/town/shop_catalog.h"
#include "ui/town/shop_tail.h"
void Shop_InitCatalogBackground(struct ShopTail *p, s32 mode)
{
    SceneInputPanel_InitBackground(&p->display, mode,
                                   ((struct ShopScriptCatalog *)p)->pageCount);
}
