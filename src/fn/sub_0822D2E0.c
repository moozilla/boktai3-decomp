#include "global.h"
#include "player.h"
struct ItemAgingDefinition {
    u8 unknown0[4];
    u16 agingLimit; /* aging state reaches agingLimit * 32 before the item changes */
    u8 unknown6[10];
};
extern const struct ItemAgingDefinition gUnk_08E61280[];
s32 Inventory_GetItemId(u32);
void Inventory_SetItemId(u32, s32);
s32 Inventory_GetItemStateValue(u32);
void Inventory_SetItemStateValue(u32, s32);
s32 Inventory_GetAgedItemReplacement(s32);
void Inventory_SetItemPackedState(u32, u32);
void Inventory_ClearItemSlot(u32);
s32 Inventory_GetItemOverrideFlag(u32);
void Inventory_SetItemOverrideFlag(u32);
static inline u32 Flags_Test(u32 *flags, u32 mask)
{
    return *flags & mask;
}
void sub_0822D2E0(s32 amount)
{
    s32 i;
    for (i = 0; i < 16; i++) {
        s32 id = Inventory_GetItemId(i);
        s32 value;
        if (id < 0 || gUnk_08E61280[id].agingLimit == 0)
            continue;
        if ((*(u8 **)gPlayers) && Flags_Test((u32 *)((*(u8 **)gPlayers) + 0x444), 0x100000))
            amount *= 2;
        value = Inventory_GetItemStateValue(i) + amount;
        if (value > 0x7FFF)
            value = 0x7FFF;
        if (value < gUnk_08E61280[id].agingLimit * 32) {
            Inventory_SetItemStateValue(i, value);
        } else if (id != 6) {
            Inventory_SetItemId(i, Inventory_GetAgedItemReplacement(id));
            Inventory_SetItemPackedState(i, 0);
        } else {
            s32 done, slot;
            Inventory_ClearItemSlot(i);
            done = 0;
            for (slot = i + 4; !done && slot <= 15; slot += 4) {
                s32 other = Inventory_GetItemId(slot);
                if (other < 0)
                    continue;
                if (other == 6 || other == 8) {
                    Inventory_SetItemId(slot, 7);
                    Inventory_SetItemPackedState(slot, 0);
                } else if (other == 0x14) {
                    Inventory_SetItemId(slot, 0x15);
                    Inventory_SetItemPackedState(slot, 0);
                } else if (other == 7 || other == 0x15 || Inventory_GetItemOverrideFlag(slot)) {
                    slot -= 4;
                    Inventory_SetItemId(slot, 8);
                    Inventory_SetItemPackedState(slot, 0);
                } else {
                    Inventory_SetItemOverrideFlag(slot);
                }
                done = 1;
            }
            if (!done) {
                slot -= 4;
                Inventory_SetItemId(slot, 8);
            }
        }
    }
}
