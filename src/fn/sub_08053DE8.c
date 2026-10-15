#include "saved_position_marker.h"
void SavedPositionMarker_SetState(struct SavedPositionMarker *marker,
                                  void (*update)(struct SavedPositionMarker *))
{
    marker->update = update;
    marker->stateFrame = 0;
}
