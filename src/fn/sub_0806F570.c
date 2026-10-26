#include "scene_effect_schedule.h"
struct SceneEffectSchedule *SceneEffectSchedule_Create(u32 argument)
{
    struct SceneEffectSchedule *manager;
    if (gSceneEffectSchedule)
        return gSceneEffectSchedule;
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
