// MATCHING_CONCESSION: register-pinning
#include "scene_inventory_catalog.h"
#define SceneInventory_BuildConsumableCatalog sub_080630FC
extern const s8 gUnk_0824DF7C[];
void SceneInventory_BuildConsumableCatalog(struct SceneInventoryScriptCatalog *input)
{
    register struct SceneInventoryScriptCatalog *p asm("r6") = input;
    register s32 page asm("r2"), next asm("r5"), count asm("r4");
    s8 *destination;
    const s8 *source;
    const s8 *table;
    register u8 *pageCount asm("r1");
    s8 *base;
    register u32 pageOffset asm("r2");
    register const s8 *tableSource asm("r4");
    p->page = 0;
    pageOffset = 0x1AB1;
    asm("" : "+r"(pageOffset));
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
    } while (page < *pageCount);
    if (page <= 6) {
        register u32 mask asm("r3") = 0xFF;
        register s32 baseOffset asm("r4");
        s8 *fillBase;
        baseOffset = 0x1AB3;
        fillBase = (s8 *)((u8 *)p + baseOffset);
        do {
            register s32 offset asm("r0") = page * 8;
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
        u32 value = sub_08227E90();
        register u32 textOffset asm("r2") = 0x1A98;
        register u32 *text asm("r1");
        text = (u32 *)((u8 *)p + textOffset);
        *text = value;
    }
}
