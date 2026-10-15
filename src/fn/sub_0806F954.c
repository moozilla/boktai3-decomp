#include "scene_range_watch.h"
#define SceneRangeWatch_CreateFromScript sub_0806F954
struct SceneRangeWatch *SceneRangeWatch_CreateFromScript(void)
{
    struct SceneRangeWatch *watch = Task_Create(8, 0x2C);
    if (watch) {
        Task_SetCallbacks(watch, (void (*)(void *))sub_0806F838, (void (*)(void *))sub_0806F86C);
        if (sub_0806F870(watch) < 0) {
            Task_Kill(watch);
            return 0;
        }
    }
    return watch;
}
