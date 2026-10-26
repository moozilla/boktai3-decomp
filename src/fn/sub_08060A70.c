#include "scene_inventory_actions.h"
#include "game_state.h"
struct SceneInventoryOwnedConsumables {
    u8 unknown0[0x160];
    s16 id;
};
s32 SceneInventory_FindConsumableAssignment(u32);
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
            struct GameState *game = gGameState;
            do {
                address <<= 1;
            } while (0);
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
    for (i = 0; i < 2; i++)
        targetBase[i] = starts[1 - i];
    SceneInput_BeginPairTransition(&((struct SceneInventoryTransitionView *)panel)->pair,
                                   enabledBase, starts, targetBase, frameBase, attributeBase, 12);
    Sound_PlaySong(0x38A);
}
