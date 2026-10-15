#include "scene_inventory_lifecycle.h"
#include "engine.h"
struct SceneInventoryLifecycleView *SceneInventory_CreatePresentation4(void)
{
    struct SceneInventoryLifecycleView *owner = Task_Create(11, 0x1D24);
    if (owner) {
        Task_SetCallbacks(owner, (void (*)(void *))SceneInventory_TickController,
                          (void (*)(void *))SceneInventory_DestroyController);
        owner->presentation = 4;
        owner->category = 1;
        if (SceneInventory_InitializeController(owner) < 0) {
            Task_Kill(owner);
            return 0;
        }
    }
    return owner;
}
