#include "terrain_edge_marker.h"
#define TerrainEdgeMarker_CreateManager sub_08056A20
struct TerrainMarkerManager *TerrainEdgeMarker_CreateManager(void)
{
    struct TerrainMarkerManager *manager = Task_Create(9, 0x2c8);
    if (manager) {
        Task_SetCallbacks(manager, (void (*)(void *))sub_08056954, (void (*)(void *))sub_080569D8);
        if (sub_08056A00(manager) < 0) {
            Task_Kill(manager);
            return 0;
        }
    }
    return manager;
}
