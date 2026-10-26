#include "terrain_edge_marker.h"
s32 TerrainEdgeMarker_UpdateVisibility(struct TerrainMarkerManager *manager)
{
    u32 visible = 0;
    u32 mode = manager->mode;
    if (mode == 0 && gGameState->unk14 != 0) {
        if (manager->blinkFrame <= 47)
            visible = 1;
        manager->blinkFrame++;
        if (manager->blinkFrame > 71)
            manager->blinkFrame = mode;
    } else
        manager->blinkFrame = visible;
    if (visible) {
        s32 i;
        for (i = 0; i < manager->count; i++)
            manager->markers[i].flags &= ~1;
    } else {
        s32 i;
        for (i = 0; i < manager->count; i++)
            manager->markers[i].flags |= 1;
    }
    return 0;
}
