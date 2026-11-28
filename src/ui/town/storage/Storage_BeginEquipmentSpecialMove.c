#include "global.h"
#include "ui/widgets/scene_input_grid.h"
#include "ui/town/storage_screen.h"
s32 Inventory_GetEquipmentId(u32);
void Storage_GetSlotPosition(struct SceneInputPosition *, u32);
void Storage_BeginEquipmentSpecialMove(struct StorageScreen *screen)
{
    struct SceneInputPosition starts[2];
    struct SceneInputPosition targets[2];
    u32 enabled[2];
    u32 frames[2];
    u32 attributes[2];
    enabled[0] = 1;
    AnimSprite_Hide(&screen->firstSlots[screen->savedSelection]);
    frames[0] = Inventory_GetEquipmentId(screen->slots[screen->savedSelection]) + 0xF0;
    attributes[0] = 0x373;
    Storage_GetSlotPosition(&starts[0], screen->savedSelection);
    Storage_GetSlotPosition(&targets[0], 32);
    enabled[1] = 0;
    frames[1] = 0xF0;
    attributes[1] = 0x373;
    starts[1].x = 0;
    starts[1].y = 0;
    starts[1].z = 0;
    targets[1].x = 0;
    targets[1].y = 0;
    targets[1].z = 0;
    SceneInput_BeginPairTransition(&screen->markers, enabled, starts, targets, frames, attributes,
                                   12);
    Sound_PlaySong(0x38A);
}
