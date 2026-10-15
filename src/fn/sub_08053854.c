#include "interaction_marker.h"
#define InteractionMarker_SetState sub_08053854
void InteractionMarker_SetState(struct InteractionMarker *marker,
                                void (*update)(struct InteractionMarker *))
{
    marker->update = update;
    marker->stateFrame = 0;
}
