#include "terrain_cell_probe.h"

#define Path_SnapToNearestPoint sub_0821EA04

u8 *Path_GetDefinition(u32 id);
s32 Path_GetPointData(u8 *def);
void Path_GetPointPosition(struct Pos16 *out, s32 data, u32 index);

s32 Path_SnapToNearestPoint(struct Pos16 *out, struct Pos16 *ref, u32 pathId)
{
    u32 minDist;
    s32 best = -1;
    u8 *def = Path_GetDefinition(pathId);
    s32 i;
    struct {
        s32 data;
    } L;
    struct Pos16 pos;
    if (!def || !def[0]) {
        out->x = 0;
        out->y = 0;
        out->z = 0;
        return best;
    }
    L.data = Path_GetPointData(def);
    if (!L.data) {
        out->x = *(u16 *)&L.data;
        out->y = *(u16 *)&L.data;
        out->z = *(u16 *)&L.data;
        return best;
    }
    minDist = best;
    for (i = 0; i < def[0]; i++) {
        s32 dx, dz;
        u32 d;
        Path_GetPointPosition(&pos, L.data, i);
        dx = (s16)ref->x - (s16)pos.x;
        dz = (s16)ref->z - (s16)pos.z;
        d = dx * dx + dz * dz;
        if (d < minDist) {
            best = i;
            minDist = d;
            out->x = pos.x;
            out->z = pos.z;
        }
    }
    Pos16_SnapToTerrain(out);
    return best;
}
