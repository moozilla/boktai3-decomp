#include "ui/town/shop_lifecycle.h"
#include "engine/engine.h"
struct SceneInventoryLifecycleView *Shop_CreatePresentation3(void)
{
    struct SceneInventoryLifecycleView *owner = Task_Create(11, 0x1D24);
    if (owner) {
        Task_SetCallbacks(owner, (void (*)(void *))Shop_TickController,
                          (void (*)(void *))Shop_DestroyController);
        owner->presentation = 3;
        owner->category = 1;
        if (Shop_InitializeController(owner) < 0) {
            Task_Kill(owner);
            return 0;
        }
    }
    return owner;
}
