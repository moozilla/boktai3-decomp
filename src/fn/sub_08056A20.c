#include "terrain_edge_marker.h"
struct TerrainMarkerManager *TerrainEdgeMarker_CreateManager(void)
{
    struct TerrainMarkerManager *manager = Task_Create(9, 0x2c8);
    if (manager) {
        Task_SetCallbacks(manager, (void (*)(void *))TerrainEdgeMarker_UpdateVisibility,
                          (void (*)(void *))TerrainEdgeMarker_DestroyManager);
        if (TerrainEdgeMarker_InitManager(manager) < 0) {
            Task_Kill(manager);
            return 0;
        }
    }
    return manager;
}
