#include "interaction_marker.h"
#define InteractionMarker_CreateFromScript sub_08053D44
struct InteractionMarker *InteractionMarker_CreateFromScript(u32 a, u32 b)
{
    struct InteractionMarker *marker = Task_Create(9, 0x118);
    if (marker) {
        Task_SetCallbacks(marker, (void (*)(void *))sub_08053AB0, (void (*)(void *))sub_08053AC4);
        if (sub_08053AEC(marker, a, b) < 0) {
            Task_Kill(marker);
            return 0;
        }
    }
    return marker;
}
