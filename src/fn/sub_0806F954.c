#include "scene_range_watch.h"
struct SceneRangeWatch *SceneRangeWatch_CreateFromScript(void)
{
    struct SceneRangeWatch *watch = Task_Create(8, 0x2C);
    if (watch) {
        Task_SetCallbacks(watch, (void (*)(void *))SceneRangeWatch_Tick,
                          (void (*)(void *))SceneRangeWatch_Destroy);
        if (SceneRangeWatch_Initialize(watch) < 0) {
            Task_Kill(watch);
            return 0;
        }
    }
    return watch;
}
