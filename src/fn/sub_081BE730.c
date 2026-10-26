#include "global.h"
#include "scene_input_grid.h"
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
    u8 unknown0[0x20];
    struct StorageWidgetFlags widgets[1];
};
void sub_081BE730(struct StoragePanel *panel)
{
    struct SceneInputPosition starts[2], targets[2];
    u32 enabled[2], frames[2], attributes[2];
    struct StorageFlagsView *display = (void *)panel;
    u32 base, attribute;
    enabled[0] = 1;
    display->widgets[panel->selection + 8].flags |= 1;
    {
        s32 id = *(s16 *)((u8 *)gGameState + panel->items[panel->selection] * 2 + 0x160);
        base = 0x121;
        frames[0] = id + base;
    }
    attribute = 0x373;
    attributes[0] = attribute;
    Storage_GetSlotPosition(starts, panel->selection);
    Storage_GetSlotPosition(targets, 32);
    enabled[1] = 0;
    frames[1] = base;
    attributes[1] = attribute;
    starts[1].x = 0;
    starts[1].y = 0;
    starts[1].z = 0;
    targets[1].x = 0;
    targets[1].y = 0;
    targets[1].z = 0;
    SceneInput_BeginPairTransition(&panel->pair, enabled, starts, targets, frames, attributes, 12);
    Sound_PlaySong(0x38A);
}
