#include "global.h"
#include "scene_marker.h"
s32 PlayerBarrier_IsActive(struct SceneMarker *marker)
{
    if (marker != 0 && marker->colliderState != 0)
        return 1;
    return 0;
}
