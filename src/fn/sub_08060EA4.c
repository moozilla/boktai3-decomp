// MATCHING_CONCESSION: register-pinning
#include "scene_inventory_actions.h"
u32 sub_0822D604(s32);
#define SceneInventory_BeginItemSpecialMove sub_08060EA4
void SceneInventory_BeginItemSpecialMove(struct SceneInventoryActionView *panel)
{
    struct SceneInputPosition starts[2], targets[2];
    u32 enabled[2], frames[2], attributes[2];
    struct SceneInventoryActionFlagsView *display = (void *)panel;
    u32 savedAttribute;
    struct SceneInputPosition *target;
    enabled[0] = 1;
    display->widgets[panel->savedSelection + 5].flags |= 1;
    frames[0] = sub_0822D604(sub_0822CDA8(panel->savedSelection));
    {
        u32 attribute = 0x373;
        savedAttribute = attribute;
        attributes[0] = attribute;
    }
    SceneInput_GetGridPosition(starts, panel->savedSelection);
    target = targets;
    SceneInput_GetGridPosition(target, 26);
    {
        register u32 *frameOut asm("r6");
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
