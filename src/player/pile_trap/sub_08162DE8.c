#include "global.h"
#include "player/pile_trap/scene_marker.h"
s32 PileTrap_IsActive(struct SceneMarker *marker)
{
    if (marker != 0 && marker->colliderState != 0)
        return 1;
    return 0;
}
