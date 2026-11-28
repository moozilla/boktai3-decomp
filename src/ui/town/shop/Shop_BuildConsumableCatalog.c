#include "ui/town/shop_catalog.h"
extern const s8 gUnk_0824DF7C[];
void Shop_BuildConsumableCatalog(struct ShopScriptCatalog *p)
{
    s32 page;
    s32 i;
    p->page = 0;
    p->pageCount = 2;
    for (page = 0; page < p->pageCount; page++) {
        for (i = 0; i < 8; i++) {
            s32 kind = gUnk_0824DF7C[page * 8 + i];
            p->catalog[page * 8 + i] = kind;
        }
    }
    for (; page <= 6; page++) {
        for (i = 0; i < 8; i++)
            p->catalog[page * 8 + i] = -1;
    }
    if (Script_SeekToKeyword(0x61))
        p->consumableText = Script_GetTextSourcePc();
}
