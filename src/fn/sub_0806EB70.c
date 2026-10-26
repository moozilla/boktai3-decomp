#include "scene_grid.h"
// Moves from one grid cell toward another along walkable cells of equal height
// (row/column scans), writing the next waypoint to *out.
// The `goto done` on the from==to path (not `return 1`) lets cross-jumping merge
// the `*out = center` store into it, as in the ROM.
#include "contact_actor.h"
s32 Div(s32, s32);
s32 Mod(s32, s32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
s32 Math_Atan2(s32, s32);
void SceneGrid_ComputeCellDelta(s32, s32, s32 *, s32 *, s32);
s32 SceneGrid_ScanRows(s32, s32, s32, s32, s32);
s32 SceneGrid_ScanColumns(s32, s32, s32, s32, s32);
s32 SceneGrid_ResolveEndpoint(struct ContactPosition *, struct ContactPosition *, s32, s32);
s32 SceneGrid_ScanTerrainDirection(s32, s32);
static inline s32 CellIndex(const struct ContactPosition *p)
{
    s32 x = p->x >> 8;
    s32 z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline u8 *Properties(s32 index)
{
    u8 *p = TerrainModifiers_FindCellOverride(index, 1);
    if (p)
        return p + 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = index * 4 + 12;
        return map->cells + offset;
    }
}
static inline void FillCenter(struct ContactPosition *position, s32 index)
{
    s32 column, row;
    column = Mod(index, SceneGrid_Width());
    row = Div(index, SceneGrid_Width());
    position->x = column * 256 + 128;
    position->y = (*Properties(index) & 15) * 256;
    position->z = row * 256 + 128;
}
s32 sub_0806EB70(struct ContactPosition *out, const struct ContactPosition *from,
                 struct ContactPosition *to, s32 rowsFirst, s32 follow)
{
    s32 fromIndex, toIndex, angle, height, column, row, result, first;
    s32 columnDelta, rowDelta;
    struct ContactPosition center, delta;
    if (!gTerrainMap)
        return 0;
    fromIndex = CellIndex(from);
    toIndex = CellIndex(to);
    height = *Properties(fromIndex) & 15;
    if (height * 256 != to->y)
        return 0;
    if (fromIndex == toIndex) {
        *out = *to;
        goto done;
    }
    FillCenter(&center, fromIndex);
    {
        struct ContactPosition *d = &delta, *c = &center;
        d->x = c->x - from->x;
        d->y = c->y - from->y;
        d->z = c->z - from->z;
    }
    if (delta.x * delta.x + delta.z * delta.z != 0) {
        *out = center;
        return 1;
    }
    {
        struct ContactPosition *c = &center;
        c->x = to->x - from->x;
        c->y = to->y - from->y;
        c->z = to->z - from->z;
    }
    angle = Math_Atan2(center.x, center.z);
    SceneGrid_ComputeCellDelta(fromIndex, toIndex, &columnDelta, &rowDelta, angle);
    column = Mod(fromIndex, SceneGrid_Width()) - 5;
    row = Div(fromIndex, SceneGrid_Width()) - 8;
    if (rowsFirst == 0) {
        if (rowDelta) {
            first = result = SceneGrid_ScanRows(column, row, fromIndex, height, rowDelta);
            if (result >= 0)
                goto found;
            result = SceneGrid_ScanColumns(column, row, fromIndex, height, columnDelta);
        resolve2:
            if (result >= 0)
                goto found;
            if (SceneGrid_ResolveEndpoint(out, to, result, first) == -1)
                return 1;
            return 0;
        }
        result = SceneGrid_ScanColumns(column, row, fromIndex, height, columnDelta);
    } else {
        if (columnDelta) {
            first = result = SceneGrid_ScanColumns(column, row, fromIndex, height, columnDelta);
            if (result >= 0)
                goto found;
            result = SceneGrid_ScanRows(column, row, fromIndex, height, rowDelta);
            goto resolve2;
        }
        result = SceneGrid_ScanRows(column, row, fromIndex, height, rowDelta);
    }
    if (result >= 0)
        goto found;
    if (SceneGrid_ResolveEndpoint(out, to, result, result) == -1)
        return 1;
    return 0;
found:
    if (result == toIndex) {
        if (follow == 1) {
            result = SceneGrid_ScanTerrainDirection(result, angle);
            FillCenter(out, result);
            *to = *out;
        } else
            *out = *to;
        return 1;
    }
    FillCenter(out, result);
done:
    return 1;
}
