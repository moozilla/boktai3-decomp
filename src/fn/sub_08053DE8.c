#include "saved_position_marker.h"
#define SavedPositionMarker_SetState sub_08053DE8
void SavedPositionMarker_SetState(struct SavedPositionMarker *marker,
                                  void (*update)(struct SavedPositionMarker *))
{
    marker->update = update;
    marker->stateFrame = 0;
}
