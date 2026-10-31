#include "scene_inventory_catalog.h"
extern const s8 gUnk_0824DF7C[];
void SceneInventory_BuildConsumableCatalog(struct SceneInventoryScriptCatalog *input)
{
    struct SceneInventoryScriptCatalog *p = input;
    s32 page, next, count;
    s8 *destination;
    const s8 *source;
    const s8 *table;
    u8 *pageCount;
    s8 *base;
    u32 pageOffset;
    const s8 *tableSource;
    p->page = 0;
    pageOffset = 0x1AB1;
    pageCount = (u8 *)p + pageOffset;
    *pageCount = 2;
    page = 0;
    tableSource = gUnk_0824DF7C;
    table = tableSource;
    {
        u32 baseOffset = 0x1AB3;
        base = (s8 *)((u8 *)p + baseOffset);
    }
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
    } while (page < (*(((u8 *)p) + pageOffset)));
    if (page <= 6) {
        u32 mask = 0xFF;
        s32 baseOffset;
        s8 *fillBase;
        baseOffset = 0x1AB3;
        fillBase = (s8 *)((u8 *)p + baseOffset);
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
        u32 textOffset = 0x1A98;
        u32 *text;
        text = (u32 *)((u8 *)p + textOffset);
        *text = value;
    }
}
