#include "saved_position_marker.h"
struct SavedPositionMarker *SavedPositionMarker_CreateFromScript(u32 a, u32 b)
{
    struct SavedPositionMarker *marker = Task_Create(9, 0xCC);
    if (marker) {
        Task_SetCallbacks(marker, (void (*)(void *))SavedPositionMarker_Tick,
                          (void (*)(void *))SavedPositionMarker_Destroy);
        if (SavedPositionMarker_InitializeFromScript(marker, a, b) < 0) {
            Task_Kill(marker);
            return 0;
        }
    }
    return marker;
}
