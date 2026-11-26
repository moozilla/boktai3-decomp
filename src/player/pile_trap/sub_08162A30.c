#include "global.h"
#include "player/pile_trap/scene_marker.h"
s32 PileTrap_Destroy(struct SceneMarker *marker)
{
    Sprite_Remove(&marker->primary);
    Sprite_Remove(&marker->secondary);
    return 0;
}
