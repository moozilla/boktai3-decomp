#include "global.h"
struct Entry {
    u32 x;
    u16 limit;
    u8 pad[10];
};
extern const struct Entry gUnk_08E61280[];
s32 Inventory_GetItemId(s32);
void Inventory_SetItemId(s32, s32), Inventory_SetItemPackedState(s32, u32);
u32 Inventory_AddItemWithState(s32 kind, s32 value)
{
    s32 i = 0;
    const struct Entry *base = gUnk_08E61280;
    const struct Entry *entry = (const struct Entry *)((u8 *)base + kind * 16);
    for (; i < 16; i++) {
        if (Inventory_GetItemId(i) < 0) {
            Inventory_SetItemId(i, kind);
            if (value < 0)
                value = 0;
            if (entry->limit * 32 < (value & 0x7fff))
                value = 0;
            Inventory_SetItemPackedState(i, value);
            return 1;
        }
    }
    return 0;
}
