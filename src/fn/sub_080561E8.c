#include "terrain_edge_marker.h"
#define TerrainEdgeMarker_AddFromScript sub_080561E8
void TerrainEdgeMarker_AddFromScript(void)
{
    union TerrainMarkerPosition position;
    s32 preserveHeight, direction, kind;
    if (Script_SeekToKeyword('p')) {
        position.packed.x = Script_GetValue();
        position.packed.y = Script_GetValue();
        position.packed.z = Script_GetValue();
    } else {
        u32 mask = 0xFFFF0000;
        position.words[0] = 0;
        position.words[1] &= mask;
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
    sub_080560C0(&position, ((direction >> 1) + 1) & 3, kind, preserveHeight);
}
