#include "interaction_marker.h"
#define InteractionMarker_Tick sub_08053AB0
u32 InteractionMarker_Tick(struct InteractionMarker *marker)
{
    sub_0824923C(marker, marker->update);
    return 0;
}
