// MATCHING_CONCESSION: register-pinning
#include "scene_inventory_actions.h"
#define SceneInventory_BeginConsumableCatalogMove sub_08060C54
void SceneInventory_BeginConsumableCatalogMove(struct SceneInventoryActionView *panel)
{
    struct SceneInputPosition starts[2], targets[2];
    u32 enabled[2], frames[2], attributes[2];
    struct SceneInventoryActionFlagsView *display = (void *)panel;
    u32 savedAttribute;
    u8 *saved;
    s32 index;
    s8 *entry;
    struct SceneInputPosition *target;
    enabled[0] = 1;
    saved = &panel->savedSelection;
    index = panel->page * 8 - 16;
    asm("" : "+r"(index));
    {
        register u32 selected asm("r3") = *saved;
        index += selected;
    }
    entry = panel->catalog;
    entry += index;
    frames[0] = *entry + 0x121;
    {
        u32 attribute = 0x373;
        savedAttribute = attribute;
        attributes[0] = attribute;
    }
    SceneInput_GetGridPosition(starts, *saved);
    target = targets;
    SceneInput_GetGridPosition(target, panel->selection);
    {
        u32 *frameOut;
        u32 placeholder;
        u32 *attributeOut;
        enabled[1] = 0;
        placeholder = 0x151;
        frameOut = frames;
        frameOut[1] = placeholder;
        attributeOut = attributes;
        attributeOut[1] = savedAttribute;
        {
            struct SceneInputPosition *start = starts;
            start[1].x = 0;
            start[1].y = 0;
            start[1].z = 0;
        }
        target[1].x = 0;
        target[1].y = 0;
        target[1].z = 0;
        SceneInput_BeginPairTransition(&((struct SceneInventoryTransitionView *)panel)->pair,
                                       enabled, starts, target, frameOut, attributeOut, 12);
    }
    Sound_PlaySong(0x38A);
}
