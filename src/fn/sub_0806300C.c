// MATCHING_CONCESSION: register-pinning
#include "scene_inventory_tail.h"
#include "scene_inventory_catalog.h"
void SceneInventory_ReadScriptCatalog(struct SceneInventoryScriptCatalog *p)
{
    s32 n;
    s8 *entry;
    p->page = 0;
    {
        register s32 minusOne asm("r2") = -1;
        register s32 fill asm("r1");
        s8 *cursor;
        asm("" : "+r"(minusOne));
        fill = minusOne;
        n = 55;
        cursor = &p->catalog[55];
        do {
            *cursor = fill;
            cursor--;
        } while (--n >= 0);
    }
    n = 0;
    if (Script_SeekToKeyword(0x54)) {
        entry = p->catalog;
        while (Script_GetPc()) {
            *entry++ = Script_GetValue();
            n++;
        }
    }
    p->itemPageCount = (n + 7) / 8;
    if (p->itemPageCount)
        n = p->itemPageCount * 8;
    if (Script_SeekToKeyword(0x52)) {
        {
            u32 offset = n + 0x1AB3;
            entry = (s8 *)(offset + (u32)p);
        }
        while (Script_GetPc()) {
            *entry++ = Script_GetValue();
            n++;
        }
    }
    p->pageCount = (n + 7) / 8;
    if (!p->pageCount)
        p->pageCount = 1;
    if (Script_SeekToKeyword(0x69))
        p->itemText = sub_08227E90();
    if (Script_SeekToKeyword(0x61))
        p->consumableText = sub_08227E90();
}
