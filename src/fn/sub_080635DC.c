#include "scene_inventory_lifecycle.h"
#define SceneInventory_CreatePresentation4 sub_080635DC
struct SceneInventoryLifecycleView *SceneInventory_CreatePresentation4(void)
{
    struct SceneInventoryLifecycleView *owner = Task_Create(11, 0x1D24);
    if (owner) {
        Task_SetCallbacks(owner, sub_080632AC, sub_080632EC);
        owner->presentation = 4;
        owner->category = 1;
        if (sub_08063334(owner) < 0) {
            Task_Kill(owner);
            return 0;
        }
    }
    return owner;
}
