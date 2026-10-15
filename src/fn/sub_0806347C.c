#include "scene_inventory_lifecycle.h"
#include "engine.h"
#define SceneInventory_CreatePresentation0 sub_0806347C
struct SceneInventoryLifecycleView *SceneInventory_CreatePresentation0(void)
{
    struct SceneInventoryLifecycleView *owner = Task_Create(11, 0x1D24);
    if (owner) {
        Task_SetCallbacks(owner, (void (*)(void *))sub_080632AC, (void (*)(void *))sub_080632EC);
        owner->presentation = 0;
        owner->category = 0;
        if (sub_08063334(owner) < 0) {
            Task_Kill(owner);
            return 0;
        }
    }
    return owner;
}
