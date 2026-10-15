// MATCHING_CONCESSION: register-pinning
#include "scene_inventory_control.h"
#define SceneInventory_ClassifyConsumableAction sub_0806122C
s32 SceneInventory_ClassifyConsumableAction(struct SceneInventoryControlView *panel)
{
    register s32 result asm("r6");
    if (panel->grid.selection > 15) {
        register u32 offset asm("r2") = 0x1AB0;
        register s32 entry asm("r5");
        s32 pageIndex;
        register s8 *entries asm("r1");
        asm("" : "+r"(offset));
        pageIndex = (*(u8 *)((u32)panel + offset) << 3) - 16;
        pageIndex += panel->grid.selection;
        offset += 3;
        entries = (void *)((u32)panel + offset);
        entries += pageIndex;
        entry = *entries;
        result = 1;
        if (entry < 0)
            goto invalid;
        {
            s32 funds = gUnk_02000710->unk7B0;
            if (funds < SceneItemValue_GetConsumableBaseValue(entry))
                goto invalid;
        }
        {
            u32 destinationOffset = 0x1AEC;
            register s16 *destination asm("r0");
            s32 current;
            destination = (void *)((u32)panel + destinationOffset);
            current = *destination;
            if (current >= 0)
                goto done;
        }
        goto invalid;
    } else {
        s32 selected = panel->grid.selection;
        register s32 item asm("r5");
        {
            struct GameState *state = gUnk_02000710;
            struct GameState *slot = (void *)(selected * 2 + (u32)state);
            item = slot->table160[0];
        }
        if (sub_0822E224(selected))
            goto invalid;
        if (item < 0)
            goto invalid;
        {
            struct SceneInventoryControlConsumable *definition = sub_0822E288(item);
            s32 value = definition->baseValue;
            result = -1;
            if (!value)
                goto invalid;
        }
    }
    goto done;
invalid:
    result = 0;
done:
    return result;
}
