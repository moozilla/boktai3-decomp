#include "hit_switch.h"
#define HitSwitch_CreateManager sub_08054AD0
struct HitSwitchManager *HitSwitch_CreateManager(void)
{
    struct HitSwitchManager *manager = Task_Create(8, 0x8fc);
    if (manager) {
        Task_SetCallbacks(manager, (void (*)(void *))sub_080546B8, (void (*)(void *))sub_08054728);
        manager->count = 0;
    }
    gUnk_02000490 = manager;
    return manager;
}
