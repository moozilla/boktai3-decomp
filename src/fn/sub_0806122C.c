// MATCHING_CONCESSION: register-pinning
#include "scene_inventory_control.h"
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
            s32 funds = gGameState->unk7B0;
            if (funds < SceneItemValue_GetConsumableBaseValue(entry))
                goto invalid;
        }
        {
            u32 destinationOffset = 0x1AEC;
            s16 *destination;
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
            struct GameState *state = gGameState;
            struct GameState *slot = (void *)(selected * 2 + (u32)state);
            item = slot->table160[0];
        }
        if (GameState_IsConsumableSlotEquipped(selected))
            goto invalid;
        if (item < 0)
            goto invalid;
        {
            struct SceneInventoryControlConsumable *definition =
                GameState_GetConsumableDescriptor(item);
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
