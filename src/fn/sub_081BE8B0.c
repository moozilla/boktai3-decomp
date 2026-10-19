#include "global.h"
#include "scene_input_grid.h"
s32 Inventory_GetEquipmentId(u32);
void Storage_GetSlotPosition(struct SceneInputPosition *, u32);
struct StoragePanel {
    u8 unknown0[0x1558];
    struct SceneInputPair pair;
    u8 unknown164C[0x1766 - 0x164C];
    u8 selection;
    u8 unknown1767[0x29];
    u8 items[1];
};
struct StorageWidgetFlags {
    u32 flags;
    u8 unknown4[0x5c];
};
struct StorageFlagsView {
    struct SceneInputWidget widgets[1];
};
void Storage_BeginEquipmentSpecialMove(struct StoragePanel *panel)
{
    struct SceneInputPosition starts[2], targets[2];
    u32 enabled[2], frames[2], attributes[2];
    struct StorageFlagsView *display = (void *)panel;
    u32 savedAttribute;
    struct SceneInputPosition *target;
    enabled[0] = 1;
    ((struct SceneInputWidget *)((u8 *)&display->widgets[panel->selection] + 0x318))->flags |= 1;
    frames[0] = Inventory_GetEquipmentId(panel->items[panel->selection]) + 0xF0;
    {
        u32 attribute = 0x373;
        savedAttribute = attribute;
        attributes[0] = attribute;
    }
    Storage_GetSlotPosition(starts, panel->selection);
    target = targets;
    Storage_GetSlotPosition(target, 32);
    {
        u32 *frameOut;
        u32 placeholder;
        u32 *attributeOut;
        enabled[1] = 0;
        placeholder = 0xF0;
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
        SceneInput_BeginPairTransition(&panel->pair, enabled, starts, target, frameOut,
                                       attributeOut, 12);
    }
    Sound_PlaySong(0x38A);
}
