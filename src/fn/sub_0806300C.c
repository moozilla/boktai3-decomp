#include "scene_inventory_catalog.h"
void SceneInventory_ReadScriptCatalog(struct SceneInventoryScriptCatalog *p)
{
    s32 n;
    p->page = 0;
    for (n = 0; n < 56; n++)
        *(p->catalog + n) = -1;
    n = 0;
    if (Script_SeekToKeyword(0x54)) {
        while (Script_GetPc()) {
            *(p->catalog + n) = Script_GetValue();
            n++;
        }
    }
    p->itemPageCount = (n + 7) / 8;
    if (p->itemPageCount)
        n = p->itemPageCount * 8;
    if (Script_SeekToKeyword(0x52)) {
        while (Script_GetPc()) {
            *(p->catalog + n) = Script_GetValue();
            n++;
        }
    }
    p->pageCount = (n + 7) / 8;
    if (!p->pageCount)
        p->pageCount = 1;
    if (Script_SeekToKeyword(0x69))
        p->itemText = Script_GetTextSourcePc();
    if (Script_SeekToKeyword(0x61))
        p->consumableText = Script_GetTextSourcePc();
}
