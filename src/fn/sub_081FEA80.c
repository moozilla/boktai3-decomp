#define RisingActorManager_EnsureCreated SunOrb_GetOrCreateManager
#define RisingActor_SetCallback          SunOrb_SetUpdateCallback
#define gRisingActorManager              gSlotController
#include "global.h"
#include "ram.h"
struct RisingActor {
    u8 pad0[0x14];
    u8 controller[0xe0];
    s16 lowered;
    u8 padF6[0x2b6];
};
struct RisingActorManager {
    u8 task[0x18];
    u32 activeSlots;
    u8 pad1C[0x1c];
    struct RisingActor actors[1];
};
void SunOrb_GetOrCreateManager(void);
void RisingActor_AdvanceHeight(void);
void SunOrb_SetUpdateCallback(void *, void (*)(void));
s32 RisingActorManager_BeginRaise(s32 index)
{
    struct RisingActorManager *manager = gSlotController;
    if (!manager) {
        SunOrb_GetOrCreateManager();
        return -1;
    }
    if (!(manager->activeSlots & (1 << index)))
        return -1;
    if (manager->actors[index].lowered)
        SunOrb_SetUpdateCallback(manager->actors[index].controller, RisingActor_AdvanceHeight);
    return 0;
}
