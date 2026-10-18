#include "saved_position_marker.h"
u32 SavedPositionMarker_Tick(struct SavedPositionMarker *marker)
{
    Callback_InvokeActor(marker, marker->update);
    return 0;
}
