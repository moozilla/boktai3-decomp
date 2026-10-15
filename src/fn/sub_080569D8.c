#include "terrain_edge_marker.h"
void Effect_Remove(void *);
s32 TerrainEdgeMarker_DestroyManager(struct TerrainMarkerManager *manager)
{
    s32 i = 0;
    if (i < manager->count) {
        struct TerrainMarkerSprite *marker = manager->markers;
        do {
            Effect_Remove(marker);
            marker++;
            i++;
        } while (i < manager->count);
    }
    return 0;
}
