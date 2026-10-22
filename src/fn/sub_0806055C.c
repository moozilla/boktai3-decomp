#include "scene_inventory_actions.h"
void SceneInventory_BeginEquipmentSwapPair(struct SceneInventoryActionView *panel)
{
    struct SceneInputPosition starts[2], targets[2];
    u32 enabled[2], frames[2], attributes[2];
    s32 i = 0;
    u32 *attributeBase;
    struct SceneInputPosition *targetBase;
    u32 *enabledBase;
    u32 *frameBase;
    u32 *attributeOut, *frameOut, *enabledOut;
    enabledBase = enabled;
    frameBase = frames;
    attributeBase = attributes;
    targetBase = targets;
    attributeOut = attributeBase;
    frameOut = frameBase;
    enabledOut = enabledBase;
    for (; i < 2; frameOut++, enabledOut++, i++) {
        u32 slot;
        s32 id, equipped;
        if (i == 0)
            slot = *(u8 *)((u8 *)panel + 0x1BEA);
        else
            slot = *(u8 *)((u8 *)panel + 0x1BE7);
        ((struct SceneInventoryActionFlagsView *)panel)->widgets[slot + 5].flags |= 1;
        equipped = SceneInventory_FindEquippedSlot(slot);
        if (equipped >= 0)
            ((struct SceneInventoryActionFlagsView *)panel)->widgets[equipped + 0x29].flags |= 1;
        id = Inventory_GetEquipmentId(slot);
        if (id != 0xFF) {
            *enabledOut = 1;
            *frameOut = id + 0xF0;
        } else {
            *enabledOut = 0;
            *frameOut = 0xF0;
        }
        *attributeOut++ = 0x373;
        SceneInput_GetGridPosition(&starts[i], slot);
    }
    for (i = 0; i < 2; i++)
        targetBase[i] = starts[1 - i];
    SceneInput_BeginPairTransition(&((struct SceneInventoryTransitionView *)panel)->pair,
                                   enabledBase, starts, targetBase, frameBase, attributeBase, 12);
    Sound_PlaySong(0x38A);
}
