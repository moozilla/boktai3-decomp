#include "ui/town/shop_catalog.h"
extern const s8 gUnk_0824DF7C[];
void Shop_BuildConsumableCatalog(struct SceneInventoryScriptCatalog *input)
{
    struct SceneInventoryScriptCatalog *p = input;
    s32 page, next, count;
    s8 *destination;
    const s8 *source;
    const s8 *table;
    s8 *base;
    const s8 *tableSource;
    p->page = 0;
    p->pageCount = 2;
    page = 0;
    tableSource = gUnk_0824DF7C;
    table = tableSource;
    base = p->catalog;
    do {
        s32 offset = page * 8;
        next = page + 1;
        destination = (s8 *)((u32)offset + (u32)base);
        source = (s8 *)((u32)offset + (u32)table);
        count = 7;
        do {
            s32 value = *source;
            *destination++ = value;
            source++;
        } while (--count >= 0);
        page = next;
    } while (page < p->pageCount);
    if (page <= 6) {
        u32 mask = 0xFF;
        s8 *fillBase;
        fillBase = p->catalog;
        do {
            s32 offset = page * 8;
            s8 *cursor;
            next = page + 1;
            cursor = (s8 *)((u32)offset + (u32)fillBase);
            count = 7;
            do {
                u32 value = *(u8 *)cursor;
                value |= mask;
                *cursor++ = value;
            } while (--count >= 0);
            page = next;
        } while (page <= 6);
    }
    if (Script_SeekToKeyword(0x61)) {
        u32 value = Script_GetTextSourcePc();
        p->consumableText = value;
    }
}
