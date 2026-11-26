#include "engine/shared/global.h"
#include "engine/shared/ram.h"
#include "player/target_registry/target_registry.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 TargetRegistry_Init(struct MotionTargets *, void *, void *);
void Task_Kill(void *);
void TargetRegistry_Update(void);
void TargetRegistry_Destroy(void);

void *TargetRegistry_Create(void *a, void *b)
{
    struct MotionTargets *result = gSceneLinkList;
    struct MotionTargets *registry;
    if (result == 0) {
        registry = Task_Create(2, sizeof(*registry));
        if (registry != 0) {
            Task_SetCallbacks(registry, TargetRegistry_Update, TargetRegistry_Destroy);
            if (TargetRegistry_Init(registry, a, b) < 0) {
                Task_Kill(registry);
                return 0;
            }
        }
        result = registry;
    }
    return result;
}
