#include "engine/shared/global.h"
#include "player/player_barrier/scene_marker.h"
void PlayerBarrier_Hide(struct SceneMarker *marker)
{
    if (marker != 0) {
        marker->colliderState = 0;
        marker->primary.flags |= 1;
    }
}
