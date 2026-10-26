#include "scene_inventory_actions.h"
#include "game_state.h"
struct SceneInventoryOwnedConsumables {
    u8 unknown0[0x160];
    s16 id;
};
void SceneInventory_BeginConsumableRemoveMove(struct SceneInventoryActionView *panel)
{
    struct SceneInputPosition starts[2], targets[2];
    u32 enabled[2], frames[2], attributes[2];
    struct SceneInventoryActionFlagsView *display = (void *)panel;
    u32 savedAttribute;
    struct SceneInputPosition *target;
    enabled[0] = 1;
    display->widgets[panel->savedSelection + 5].flags |= 1;
    {
        u32 address = panel->savedSelection;
        struct GameState *game;
        game = gGameState;
        address <<= 1;
        address += (u32)game;
        frames[0] = ((struct SceneInventoryOwnedConsumables *)address)->id + 0x121;
    }
    {
        u32 attribute = 0x373;
        savedAttribute = attribute;
        attributes[0] = attribute;
    }
    SceneInput_GetGridPosition(starts, panel->savedSelection);
    target = targets;
    SceneInput_GetGridPosition(target, 24);
    {
        u32 *frameOut;
        u32 placeholder;
        u32 *attributeOut;
        enabled[1] = 0;
        placeholder = 0x121;
        frameOut = frames;
        frameOut[1] = placeholder;
        attributeOut = attributes;
        attributeOut[1] = savedAttribute;
        {
            struct SceneInputPosition *start = starts;
            start[1].x = 0;
            starts[1].y = 0;
            starts[1].z = 0;
        }
        target[1].x = 0;
        target[1].y = 0;
        target[1].z = 0;
        SceneInput_BeginPairTransition(&((struct SceneInventoryTransitionView *)panel)->pair,
                                       enabled, starts, target, frameOut, attributeOut, 12);
    }
    Sound_PlaySong(0x38A);
}
