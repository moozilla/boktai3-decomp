#include "engine/shared/scene_inventory_actions.h"
u32 Selection_GetDisplayFrame(s32);
void SceneInventory_BeginItemCatalogMove(struct SceneInventoryActionView *panel)
{
    struct SceneInputPosition starts[2], targets[2];
    u32 enabled[2], frames[2], attributes[2];
    u8 *saved;
    s32 index;
    s8 *entry;
    struct SceneInputPosition *target;
    enabled[0] = 1;
    saved = &panel->savedSelection;
    index = panel->page * 8 - 16;
    {
        u32 selected = *saved;
        index += selected;
    }
    entry = panel->catalog;
    entry += index;
    frames[0] = Selection_GetDisplayFrame(*entry);
    attributes[0] = 0x373;
    SceneInput_GetGridPosition(starts, *saved);
    target = targets;
    SceneInput_GetGridPosition(target, panel->selection);
    enabled[1] = 0;
    frames[1] = 0x151;
    attributes[1] = 0x373;
    {
        struct SceneInputPosition *start = starts;
        start[1].x = 0;
        start[1].y = 0;
        start[1].z = 0;
    }
    target[1].x = 0;
    target[1].y = 0;
    target[1].z = 0;
    SceneInput_BeginPairTransition(&((struct SceneInventoryTransitionView *)panel)->pair, enabled,
                                   starts, target, frames, attributes, 12);
    Sound_PlaySong(0x38A);
}
