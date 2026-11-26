#include "ui/town/shop_lifecycle.h"
#include "engine/engine.h"
struct ShopLifecycleView *Shop_CreateWeaponShop(void)
{
    struct ShopLifecycleView *owner = Task_Create(11, 0x1D24);
    if (owner) {
        Task_SetCallbacks(owner, (void (*)(void *))Shop_TickController,
                          (void (*)(void *))Shop_DestroyController);
        owner->presentation = 0;
        owner->category = 0;
        if (Shop_InitializeController(owner) < 0) {
            Task_Kill(owner);
            return 0;
        }
    }
    return owner;
}
