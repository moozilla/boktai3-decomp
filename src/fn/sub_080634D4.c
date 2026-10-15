#include "scene_inventory_lifecycle.h"
#include "engine.h"
#define SceneInventory_CreatePresentation1 sub_080634D4
struct SceneInventoryLifecycleView *SceneInventory_CreatePresentation1(void)
{
    struct SceneInventoryLifecycleView *owner = Task_Create(11, 0x1D24);
    if (owner) {
        Task_SetCallbacks(owner, (void (*)(void *))sub_080632AC, (void (*)(void *))sub_080632EC);
        owner->presentation = 1;
        owner->category = 1;
        if (sub_08063334(owner) < 0) {
            Task_Kill(owner);
            return 0;
        }
    }
    return owner;
}
