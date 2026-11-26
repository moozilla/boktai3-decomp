#include "global.h"
#include "player/effects/scene_marker.h"
s32 PlayerBarrier_Destroy(struct SceneMarker *marker)
{
    Sprite_Remove(&marker->primary);
    Sprite_Remove(&marker->secondary);
    return 0;
}
