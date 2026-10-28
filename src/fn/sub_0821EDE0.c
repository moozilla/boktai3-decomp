#include "terrain_cell_probe.h"
#include "ram.h"

u8 *Path_GetDefinition(u32 id);
s32 Path_GetPointPositionById(u16 *, u32, u32);
s32 Mod(s32, s32);

s32 Path_SnapToRandomFarPoint(struct Pos16 *out, struct Pos16 *ref, u32 pathId, u32 minDist)
{
    u8 *def = Path_GetDefinition(pathId);
    s32 start, i;
    struct Pos16 pos;
    struct {
        s32 idx;
    } L;
    if (!def || !def[0]) {
        out->x = 0;
        out->y = 0;
        out->z = 0;
        return -1;
    }
    {
        u32 r = (gRandomIndex + 1) & 0x3FF;
        gRandomIndex = r;
        start = Mod(*(const u16 *)((u8 *)gRandomTable + r * 2), def[0]);
    }
    for (i = start; i < start + def[0]; i++) {
        s32 dx, dz;
        L.idx = Mod(i, def[0]);
        Path_GetPointPositionById((u16 *)&pos, pathId, L.idx);
        dx = (s16)ref->x - (s16)pos.x;
        dz = (s16)ref->z - (s16)pos.z;
        if ((u32)(dx * dx + dz * dz) >= minDist) {
            out->x = pos.x;
            out->z = pos.z;
            Pos16_SnapToTerrain(out);
            return L.idx;
        }
    }
    return start;
}
