#include "scene_inventory_actions.h"
u32 Selection_GetDisplayFrame(s32);
void SceneInventory_BeginItemSpecialMove(struct SceneInventoryActionView *panel)
{
    struct SceneInputPosition starts[2], targets[2];
    u32 enabled[2], frames[2], attributes[2];
    struct SceneInventoryActionFlagsView *display = (void *)panel;
    struct SceneInputPosition *target;
    enabled[0] = 1;
    display->widgets[panel->savedSelection + 5].flags |= 1;
    frames[0] = Selection_GetDisplayFrame(Inventory_GetItemId(panel->savedSelection));
    attributes[0] = 0x373;
    SceneInput_GetGridPosition(starts, panel->savedSelection);
    target = targets;
    SceneInput_GetGridPosition(target, 26);
    enabled[1] = 0;
    frames[1] = 0x151;
    attributes[1] = 0x373;
    starts[1].x = 0;
    starts[1].y = 0;
    starts[1].z = 0;
    target[1].x = 0;
    target[1].y = 0;
    target[1].z = 0;
    SceneInput_BeginPairTransition(&((struct SceneInventoryTransitionView *)panel)->pair, enabled,
                                   starts, target, frames, attributes, 12);
    Sound_PlaySong(0x38A);
}
