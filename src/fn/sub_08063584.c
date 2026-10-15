#include "scene_inventory_lifecycle.h"
#define SceneInventory_CreatePresentation3 sub_08063584
struct SceneInventoryLifecycleView *SceneInventory_CreatePresentation3(void)
{
    struct SceneInventoryLifecycleView *owner = Task_Create(11, 0x1D24);
    if (owner) {
        Task_SetCallbacks(owner, sub_080632AC, sub_080632EC);
        owner->presentation = 3;
        owner->category = 1;
        if (sub_08063334(owner) < 0) {
            Task_Kill(owner);
            return 0;
        }
    }
    return owner;
}
