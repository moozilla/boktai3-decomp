#include "global.h"
#include "player/pile_trap/scene_marker.h"
void PileTrap_Hide(struct SceneMarker *marker)
{
    if (marker != 0) {
        marker->colliderState = 0;
        marker->primary.flags |= 1;
    }
}
