#include "saved_position_marker.h"
#define SavedPositionMarker_Tick sub_08053FF8
u32 SavedPositionMarker_Tick(struct SavedPositionMarker *marker)
{
    sub_0824923C(marker, marker->update);
    return 0;
}
