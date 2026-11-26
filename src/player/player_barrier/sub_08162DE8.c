#include "engine/shared/global.h"
#include "player/player_barrier/scene_marker.h"
s32 PlayerBarrier_IsActive(struct SceneMarker *marker)
{
    if (marker != 0 && marker->colliderState != 0)
        return 1;
    return 0;
}
