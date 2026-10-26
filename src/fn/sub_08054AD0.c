#include "hit_switch.h"
struct HitSwitchManager *HitSwitch_CreateManager(void)
{
    struct HitSwitchManager *manager = Task_Create(8, 0x8fc);
    if (manager) {
        Task_SetCallbacks(manager, (void (*)(void *))HitSwitch_UpdateManager,
                          (void (*)(void *))HitSwitch_DestroyManager);
        manager->count = 0;
    }
    gHitSwitchManager = manager;
    return manager;
}
