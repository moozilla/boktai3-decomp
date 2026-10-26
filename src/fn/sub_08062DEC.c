// MATCHING_CONCESSION: register-pinning
#include "scene_inventory_catalog.h"
extern const s8 gUnk_0824DF3C[];
struct SceneInventoryUnlockView {
    u8 unknown0[0x748];
    u32 low, high;
};
void SceneInventory_BuildUnlockedEquipmentCatalog(struct SceneInventoryScriptCatalog *input)
{
    struct SceneInventoryScriptCatalog *p = input;
    s32 page;
    s32 next;
    s32 count;
    s8 *destination;
    const s8 *source;
    struct GameState **location;
    register u32 one asm("r8");
    u8 *pageCount;
    s8 *volatile base;
    u8 *initialCount;
    p->page = 0;
    initialCount = &p->pageCount;
    *initialCount = 6;
    page = 0;
    location = &gGameState;
    one = 1;
    base = p->catalog;
    pageCount = initialCount;
    do {
        s32 offset = page * 8;
        next = page + 1;
        {
            register s8 *baseLoad asm("r1") = base;
            destination = (s8 *)((u32)offset + (u32)baseLoad);
        }
        source = (s8 *)((u32)offset + (u32)gUnk_0824DF3C);
        count = 7;
        do {
            s32 kind = *source;
            u32 enabled;
            *(u8 *)destination = 0xFF;
            if (kind >= 0) {
                if (kind <= 31) {
                    u32 *flags = (u32 *)*location;
                    register u32 offset asm("r0");
                    offset = 0x748;
                    flags = (u32 *)((u8 *)flags + offset);
                    offset = one << kind;
                    enabled = *flags & offset;
                } else {
                    struct GameState **copy = location;
                    u32 *flags;
                    u32 offset;
                    flags = (u32 *)*copy;
                    offset = 0x74C;
                    flags = (u32 *)((u8 *)flags + offset);
                    enabled = kind - 32;
                    offset = one << enabled;
                    enabled = *flags & offset;
                }
                if (enabled)
                    *destination = kind;
            }
            destination++;
            source++;
        } while (--count >= 0);
        page = next;
    } while (page < *pageCount);
    if (page <= 6) {
        u32 mask = 0xFF;
        s8 *fillBase = p->catalog;
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
    if (Script_SeekToKeyword(0x77))
        p->equipmentText = Script_GetTextSourcePc();
    if (Script_SeekToKeyword(0x63)) {
        p->equipmentLabel = Script_GetTextSourcePc();
        if (p->equipmentLabel) {
            p->equipmentLabel =
                (u32)Text_LookupString(Script_ParseStringRef((void *)p->equipmentLabel) + 1);
        }
    }
}
