#include "engine/shared/global.h"
#include "player/player_barrier/scene_marker.h"
s32 PlayerBarrier_Destroy(struct SceneMarker *marker)
{
    Sprite_Remove(&marker->primary);
    Sprite_Remove(&marker->secondary);
    return 0;
}
