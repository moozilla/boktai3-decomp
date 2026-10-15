#include "saved_position_marker.h"
#define SavedPositionMarker_IsStoredPositionNearby sub_08053D8C
u32 SavedPositionMarker_IsStoredPositionNearby(struct SavedPositionMarker *marker)
{
    struct GameState *state = gUnk_02000710;
    s32 dx = state->positionX - marker->position.x;
    s32 dz;
    if (dx < 0)
        dx = -dx;
    dz = state->positionZ - marker->position.z;
    if (dz < 0)
        dz = -dz;
    if (dx > 96 || dz > 96)
        return 0;
    if (dx * dx + dz * dz > 9216)
        return 0;
    return 1;
}
