#include "terrain_edge_marker.h"
void ResourceRegistry_RegisterResource(u32, void *, u32);
s32 TerrainEdgeMarker_InitManager(struct TerrainMarkerManager *manager)
{
    manager->mode = 0;
    manager->count = 0;
    manager->blinkFrame = 0;
    ResourceRegistry_RegisterResource(0xFD22, manager, 0);
    return 0;
}
