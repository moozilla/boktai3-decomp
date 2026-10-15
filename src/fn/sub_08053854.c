#include "interaction_marker.h"
void InteractionMarker_SetState(struct InteractionMarker *marker,
                                void (*update)(struct InteractionMarker *))
{
    marker->update = update;
    marker->stateFrame = 0;
}
