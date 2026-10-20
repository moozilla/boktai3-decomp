#include "scene_inventory_actions.h"
#ifdef NONMATCHING
// fuzzy: 93%
// Open: the slot index lands in r5 and the widget base in r4 (swapped in the
// original); pair-copy loop counter ordering as in sub_08060DB4.
struct SceneInventoryOwnedConsumables {
    u8 unknown0[0x160];
    s16 id;
};
extern struct GameState *gUnk_02000710;
s32 SceneInventory_FindConsumableAssignment(u32);
#define SceneInventory_BeginConsumableSwapPair sub_08060A70
void SceneInventory_BeginConsumableSwapPair(struct SceneInventoryActionView *panel)
{
    struct SceneInputPosition starts[2], targets[2];
    u32 enabled[2], frames[2], attributes[2];
    s32 i = 0;
    struct SceneInputPosition *targetBase;
    u32 *enabledBase;
    u32 *frameBase;
    u32 *attributeBase;
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
        s32 id, assigned;
        if (i == 0)
            slot = *(u8 *)((u8 *)panel + 0x1BEA);
        else
            slot = *(u8 *)((u8 *)panel + 0x1BE7);
        ((struct SceneInventoryActionFlagsView *)panel)->widgets[slot + 5].flags |= 1;
        assigned = SceneInventory_FindConsumableAssignment(slot);
        if (assigned >= 0)
            ((struct SceneInventoryActionFlagsView *)panel)->widgets[assigned + 0x29].flags |= 1;
        {
            u32 address = slot;
            struct GameState *game = gUnk_02000710;
            address <<= 1;
            address += (u32)game;
            id = ((struct SceneInventoryOwnedConsumables *)address)->id;
        }
        if (id >= 0) {
            *enabledOut = 1;
            *frameOut = id + 0x121;
        } else {
            *enabledOut = 0;
            *frameOut = 0x121;
        }
        *attributeOut++ = 0x373;
        SceneInput_GetGridPosition(&starts[i], slot);
    }
    i = 1;
    do {
        targetBase[1 - i] = starts[i];
        i--;
    } while (i >= 0);
    SceneInput_BeginPairTransition(&((struct SceneInventoryTransitionView *)panel)->pair,
                                   enabledBase, starts, targetBase, frameBase, attributeBase, 12);
    Sound_PlaySong(0x38A);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08060A70);
#endif
