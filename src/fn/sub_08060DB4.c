#include "scene_inventory_actions.h"
u32 Selection_GetDisplayFrame(s32);
void SceneInventory_BeginItemSwapPair(struct SceneInventoryActionView *panel)
{
    struct SceneInputPosition starts[2], targets[2];
    u32 enabled[2], frames[2], attributes[2];
    s32 i = 0;
    u32 *enabledBase = enabled;
    u32 *frameBase = frames;
    u32 *attributeBase = attributes;
    struct SceneInputPosition *targetBase = targets;
    u32 *attributeOut = attributeBase;
    u32 *frameOut = frameBase;
    u32 *enabledOut = enabledBase;
    for (; i < 2; enabledOut++, i++) {
        u32 slot;
        s32 id;
        if (i == 0)
            slot = *(u8 *)((u8 *)panel + 0x1BEA);
        else
            slot = *(u8 *)((u8 *)panel + 0x1BE7);
        ((struct SceneInventoryActionFlagsView *)panel)->widgets[slot + 5].flags |= 1;
        id = Inventory_GetItemId(slot);
        if (id >= 0)
            *enabledOut = 1;
        else
            *enabledOut = 0;
        *frameOut++ = Selection_GetDisplayFrame(id);
        *attributeOut++ = 0x373;
        SceneInput_GetGridPosition(&starts[i], slot);
    }
    for (i = 0; i < 2; i++)
        targetBase[i] = starts[1 - i];
    SceneInput_BeginPairTransition(&((struct SceneInventoryTransitionView *)panel)->pair,
                                   enabledBase, starts, targetBase, frameBase, attributeBase, 12);
    Sound_PlaySong(0x38A);
}
