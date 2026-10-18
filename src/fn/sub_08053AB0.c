#include "interaction_marker.h"
u32 InteractionMarker_Tick(struct InteractionMarker *marker)
{
    Callback_InvokeActor(marker, marker->update);
    return 0;
}
