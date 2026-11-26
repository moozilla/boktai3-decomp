#include "ui/town/shop_actions.h"
#include "system/game_state/game_state.h"
struct SceneInventoryOwnedConsumables {
    u8 unknown0[0x160];
    s16 id;
};
s32 Shop_FindConsumableAssignment(u32);
void Shop_BeginConsumableSwapPair(struct SceneInventoryActionView *panel)
{
    struct SceneInputPosition starts[2], targets[2];
    u32 enabled[2], frames[2], attributes[2];
    s32 i = 0;
    struct SceneInputPosition *targetBase;
    u32 *enabledBase;
    u32 *frameBase;
    u32 *attributeBase;
    enabledBase = enabled;
    frameBase = frames;
    attributeBase = attributes;
    targetBase = targets;
    for (; i < 2; i++) {
        u32 slot;
        s32 id, assigned;
        if (i == 0)
            slot = panel->savedSelection;
        else
            slot = panel->selection;
        ((struct SceneInventoryActionFlagsView *)panel)->widgets[slot + 5].flags |= 1;
        assigned = Shop_FindConsumableAssignment(slot);
        if (assigned >= 0)
            ((struct SceneInventoryActionFlagsView *)panel)->widgets[assigned + 0x29].flags |= 1;
        {
            u32 address = slot;
            struct GameState *game = gGameState;
            address <<= 1;
            address += (u32)game;
            id = ((struct SceneInventoryOwnedConsumables *)address)->id;
        }
        if (id >= 0) {
            enabled[i] = 1;
            frames[i] = id + 0x121;
        } else {
            enabled[i] = 0;
            frames[i] = 0x121;
        }
        attributes[i] = 0x373;
        SceneInput_GetGridPosition(&starts[i], slot);
    }
    for (i = 0; i < 2; i++)
        targetBase[i] = starts[1 - i];
    SceneInput_BeginPairTransition(&((struct SceneInventoryTransitionView *)panel)->pair,
                                   enabledBase, starts, targetBase, frameBase, attributeBase, 12);
    Sound_PlaySong(0x38A);
}
