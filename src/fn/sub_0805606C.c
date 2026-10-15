#include "terrain_edge_marker.h"
#define TerrainEdgeMarker_SetModeFromScript sub_0805606C
void TerrainEdgeMarker_SetModeFromScript(void)
{
    if (Script_SeekToKeyword('m')) {
        struct TerrainMarkerManager *manager = sub_0821A2C4(0xFD22);
        if (manager) {
            manager->mode = Script_GetValue();
            if (manager->mode == 1) {
                s32 i;
                for (i = 0; i < manager->count; i++)
                    manager->markers[i].flags |= 1;
                manager->blinkFrame = 0;
            }
        }
    }
}
