#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "objects/floor_switch/floor_switch_layout.h"

s32 FloorSwitch_InitManager(struct FloorSwitchManager *);
u32 SpriteSlots_UpdateActive(struct FloorSwitchManager *);
u32 FloorSwitch_OnManagerDestroy(struct FloorSwitchManager *);

struct FloorSwitchManager *FloorSwitch_GetOrCreateManager(void)
{
    struct FloorSwitchManager *manager;

    if (gFloorSwitchManager != 0)
        return (struct FloorSwitchManager *)gFloorSwitchManager;
    manager = Task_Create(8, sizeof(*manager));
    if (manager != 0) {
        Task_SetCallbacks(manager, (void (*)(void *))SpriteSlots_UpdateActive,
                          (void (*)(void *))FloorSwitch_OnManagerDestroy);
        if (FloorSwitch_InitManager(manager) < 0) {
            Task_Kill(manager);
            return 0;
        }
    }
    return manager;
}
