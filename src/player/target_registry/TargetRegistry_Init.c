#include "engine/ram.h"
#include "player/target_registry/target_registry.h"

s32 TargetRegistry_Init(struct MotionTargets *registry, void *unusedA, void *unusedB)
{
    registry->first = NULL;
    registry->unknown1C = 0;
    gSceneLinkList = registry;
    return 0;
}
