#include "terrain_edge_marker.h"
void sub_0821A284(u32, void *, u32);
s32 TerrainEdgeMarker_InitManager(struct TerrainMarkerManager *manager)
{
    manager->mode = 0;
    manager->count = 0;
    manager->blinkFrame = 0;
    sub_0821A284(0xFD22, manager, 0);
    return 0;
}
