#include "saved_position_marker.h"
#define SavedPositionMarker_CreateFromScript sub_08054250
struct SavedPositionMarker *SavedPositionMarker_CreateFromScript(u32 a, u32 b)
{
    struct SavedPositionMarker *marker = Task_Create(9, 0xCC);
    if (marker) {
        Task_SetCallbacks(marker, (void (*)(void *))sub_08053FF8, (void (*)(void *))sub_0805400C);
        if (sub_08054028(marker, a, b) < 0) {
            Task_Kill(marker);
            return 0;
        }
    }
    return marker;
}
