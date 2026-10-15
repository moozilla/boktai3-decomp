#include "scene_effect_schedule.h"
struct SceneEffectSchedule *SceneEffectSchedule_Create(u32 argument)
{
    struct SceneEffectSchedule *manager;
    if (gUnk_0200019C)
        return gUnk_0200019C;
    manager = Task_Create(10, sizeof(*manager));
    if (manager) {
        Task_SetCallbacks(manager, (void (*)(void *))SceneEffectSchedule_Tick,
                          (void (*)(void *))SceneEffectSchedule_Destroy);
        if (SceneEffectSchedule_Initialize(manager, argument) < 0) {
            Task_Kill(manager);
            return 0;
        }
    }
    return manager;
}
