#include "interaction_marker.h"
struct InteractionMarker *InteractionMarker_CreateFromScript(u32 a, u32 b)
{
    struct InteractionMarker *marker = Task_Create(9, 0x118);
    if (marker) {
        Task_SetCallbacks(marker, (void (*)(void *))InteractionMarker_Tick,
                          (void (*)(void *))InteractionMarker_Destroy);
        if (InteractionMarker_InitializeFromScript(marker, a, b) < 0) {
            Task_Kill(marker);
            return 0;
        }
    }
    return marker;
}
