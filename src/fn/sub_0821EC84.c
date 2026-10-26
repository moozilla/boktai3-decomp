#include "global.h"
#include "engine.h"
struct Position {
    s16 x, y, z, pad;
};
u8 *Path_GetDefinition(u32);
void *Path_GetPointData(u8 *);
void Path_GetPointPosition(struct Position *, void *, u32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
static inline s32 CellIndex(struct Position *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline u8 *CellData(s32 i)
{
    u8 *p = TerrainModifiers_FindCellOverride(i, 1);
    if (p)
        return p + 4;
    return gTerrainMap->cells + (i * 4 + 12);
}
static inline s32 Height(struct Position *p)
{
    u8 *data = CellData(CellIndex(p));
    s32 shape = *data >> 4, h = (*data & 15) << 8;
    switch (shape) {
        case 1:
            h -= (u8)p->z;
            break;
        case 2:
            h -= (u8)p->x;
            break;
    }
    return h;
}
// confidence=low: selects the nearest path point outside the minimum squared distance.
s32 Path_FindNearestDistantPoint(struct Position *out, struct Position *from, u32 id, u32 minArg)
{
    u32 minimum = minArg;
    struct Position point;
    s32 chosen = -1;
    u8 *def = Path_GetDefinition(id);
    void *data;
    u32 best;
    s32 i;
    if (!def || !*def) {
        out->x = 0;
        out->y = 0;
        out->z = 0;
        return chosen;
    }
    data = Path_GetPointData(def);
    if (!data) {
        out->x = 0;
        out->y = 0;
        out->z = 0;
        return chosen;
    }
    Path_GetPointPosition(&point, data, 0);
    out->x = point.x;
    out->z = point.z;
    best = chosen;
    for (i = 0; i < *def; i++) {
        s32 x, z;
        u32 distance;
        Path_GetPointPosition(&point, data, i);
        x = from->x - point.x;
        z = from->z - point.z;
        distance = x * x + z * z;
        if (distance < best && distance >= minimum) {
            chosen = i;
            best = distance;
            out->x = point.x;
            out->z = point.z;
        }
    }
    out->y = Height(out);
    return chosen;
}
