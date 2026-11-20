#include "terrain_edge_marker.h"
void TerrainEdgeMarker_AddFromScript(void)
{
    union TerrainMarkerPosition position;
    s32 preserveHeight, direction, kind;
    if (Script_SeekToKeyword('p')) {
        position.packed.x = Script_GetValue();
        position.packed.y = Script_GetValue();
        position.packed.z = Script_GetValue();
    } else {
        position.packed.x = 0;
        position.packed.y = 0;
        position.packed.z = 0;
    }
    if (Script_SeekToKeyword('f'))
        preserveHeight = Script_GetValue();
    else
        preserveHeight = 0;
    if (Script_SeekToKeyword('d'))
        direction = Script_GetValue();
    else
        direction = 0;
    if (Script_SeekToKeyword('t'))
        kind = Script_GetValue();
    else
        kind = 0;
    TerrainEdgeMarker_Add(&position, ((direction >> 1) + 1) & 3, kind, preserveHeight);
}
