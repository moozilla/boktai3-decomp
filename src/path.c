#define Path_SelectPoint                    sub_0821F0C4
#define Path_GetPointNodeIdById             sub_0821E840
#define Path_GetPointFacingById             sub_0821E820
#define Route_GetCurrentPointXZ             sub_0821E7D0
#define Path_GetPointActionKind             sub_0821E7C4
#define Path_GetPointHeightOffset           sub_0821E7B4
#define Route_GetMovementTowardCurrentPoint sub_0821E67C
#define Route_GetNextPoint                  sub_0821E654
#include "path.h"
#include "scene_actor_route.h"
#include "engine.h"
#include "terrain_cell_probe.h"
#include "ram.h"

struct SceneActorRoutePoint *Route_GetNextPoint(struct SceneActorRouteCursor *cursor)
{
    s32 next;
    struct SceneActorRouteDefinition *definition;
    u8 *base;
    next = cursor->pointIndex;
    next++;
    definition = cursor->definition;
    if (next >= definition->pointCount)
        next = 0;
    base = gTerrainMap->routes;
    return (struct SceneActorRoutePoint *)(base + definition->pointOffset +
                                           next * sizeof(struct SceneActorRoutePoint));
}

extern const s16 gUnk_086149C4[];

s32 Math_Atan2(s32, s32);

static inline s32 scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

s32 Route_GetMovementTowardCurrentPoint(struct SceneActorRouteCursor *ctx, s16 *pos, s16 *out,
                                        u8 *dir, u32 rangeX, u32 rangeZ, s32 speed)
{
    struct TerrainProbe t;
    s32 dx, dz;
    u8 angle;
    u32 a;

    t.x = ctx->point->x;
    t.z = ctx->point->z;
    dx = (s16)t.x - pos[0];
    dz = (s16)t.z - pos[2];
    angle = Math_Atan2(dx, dz);
    *dir = angle;
    a = dx;
    if (dx < 0)
        a = -dx;
    if (a <= rangeX) {
        a = dz;
        if (dz < 0)
            a = -dz;
        if (a <= rangeZ) {
            out[0] = dx;
            out[2] = dz;
            return 1;
        }
    }
    out[0] = scale(gUnk_086149C4[(angle + 0x40) & 0xff] * speed);
    out[2] = scale(gUnk_086149C4[angle] * speed);
    return 0;
}

struct SceneActorRouteDefinition *Path_GetDefinition(u32 i)
{
    struct PathResource *resource = gTerrainMap->routes;
    return i < resource->count ? &resource->definitions[i] : 0;
}

struct SceneActorRoutePoint *Path_GetPointData(struct SceneActorRouteDefinition *definition)
{
    u8 *base = gTerrainMap->routes;
    return (struct SceneActorRoutePoint *)(base + definition->pointOffset);
}

u16 Terrain_GetHeightAt(void *);

void Path_GetPointPosition(struct Pos16 *out, struct SceneActorRoutePoint *points, u32 index)
{
    struct SceneActorRoutePoint *point = &points[index];
    out->x = point->x;
    out->z = point->z;
    out->y = Terrain_GetHeightAt(out);
}

u32 Path_GetPointFacing(struct SceneActorRoutePoint *points, u32 index)
{
    return points[index].nodeAndFacing >> 8;
}

u32 Path_GetPointNodeId(struct SceneActorRoutePoint *points, u32 index)
{
    return *(u8 *)&points[index].nodeAndFacing;
}

/* Path_GetPointHeightOffset: Extracts the point's low attribute nibble as a fixed-point height offset tentatively */

u32 Path_GetPointHeightOffset(struct SceneActorRoutePoint *points, s32 i)
{
    return (points[i].attributes & 15) << 8;
}

/* Path_GetPointActionKind: Extracts the high attribute nibble used to select point arrival behavior */

u32 Path_GetPointActionKind(struct SceneActorRoutePoint *points, s32 i)
{
    return points[i].attributes >> 4;
}

void Route_GetCurrentPointXZ(struct Pos16 *destination, struct SceneActorRouteCursor *cursor)
{
    struct SceneActorRoutePoint *point = cursor->point;
    destination->x = point->x;
    destination->z = point->z;
}

u32 Route_GetCurrentPointFacing(struct SceneActorRouteCursor *cursor)
{
    return cursor->point->nodeAndFacing >> 8;
}

u32 Route_GetCurrentPointNodeId(struct SceneActorRouteCursor *cursor)
{
    return *(u8 *)&cursor->point->nodeAndFacing;
}

s32 Path_GetPointPositionById(struct Pos16 *a, u32 b, u32 c)
{
    s32 r;
    s32 x;
    x = (s32)Path_GetPointData(Path_GetDefinition(b));
    if (x != 0) {
        Path_GetPointPosition(a, (struct SceneActorRoutePoint *)x, c);
        r = 0;
    } else {
        a->x = x;
        a->y = x;
        a->z = x;
        r = -1;
    }
    return r;
}

s32 Path_GetPointFacingById(s32 path, s32 point)
{
    void *data = Path_GetPointData(Path_GetDefinition(path));
    s32 result;
    if (data != NULL)
        result = Path_GetPointFacing(data, point);
    else
        result = 0;
    return result;
}

s32 Path_GetPointNodeIdById(s32 path, s32 point)
{
    void *data = Path_GetPointData(Path_GetDefinition(path));
    s32 result;
    if (data != NULL)
        result = Path_GetPointNodeId(data, point);
    else
        result = 0;
    return result;
}

s32 Path_GetPointFromScript(void)
{
    s16 vector[4];
    struct ScriptUtilityReference ref;
    s16 *saved;
    u32 first = Script_GetValue();
    u32 second = Script_GetValue();
    saved = vector;
    if (Path_GetPointPositionById((struct Pos16 *)vector, first, second) < 0)
        return -1;
    Script_ReadReference(&ref);
    Script_WriteReferenceValue(&ref, 0, vector[0]);
    Script_ReadReference(&ref);
    Script_WriteReferenceValue(&ref, 0, saved[1]);
    Script_ReadReference(&ref);
    Script_WriteReferenceValue(&ref, 0, saved[2]);
    return 0;
}

union RouteCoordinateWord {
    struct {
        s16 value, padding;
    } coordinate;
    u32 word;
};

struct RouteBoundsPoint {
    union RouteCoordinateWord x, z;
};

u32 SceneActorRoute_IsOutsidePointBounds(struct SceneActorRouteCursor *cursor,
                                         const struct ContactPosition *position, u32 xBound,
                                         u32 zBound)
{
    struct RouteBoundsPoint point;
    s32 dx, dz;
    u32 magnitude;
    point.x.coordinate.value = cursor->point->x;
    point.z.coordinate.value = cursor->point->z;
    {
        const s16 pointZ[1] = {point.z.coordinate.value};
        dx = position->x - point.x.coordinate.value;
        if (dx < 0)
            dx = -dx;
        magnitude = dx;
        if (magnitude > xBound)
            return 1;
        dz = position->z - (s16)pointZ[0];
        magnitude = dz;
        if (dz < 0)
            magnitude = -dz;
        if (magnitude > zBound)
            return 1;
        return 0;
    }
}

s32 Mod(s32, s32);

u8 *TerrainModifiers_FindCellOverride(s32, s32);

s32 Path_PickRandomPoint(struct Pos16 *out, u32 pathId)
{
    struct SceneActorRouteDefinition *def = Path_GetDefinition(pathId);
    s32 point;
    s32 x, z, cell, height;
    u8 *props;
    u32 surface;
    if (def == 0 || def->pointCount == 0) {
        out->x = 0;
        out->y = 0;
        out->z = 0;
        return -1;
    }
    gRandomIndex = (gRandomIndex + 1) & 1023;
    {
        s32 sample = ((const u16 *)gRandomTable)[gRandomIndex];
        point = Mod(sample, def->pointCount);
    }
    Path_GetPointPositionById(out, pathId, point);
    x = (s16)out->x >> 8;
    z = (s16)out->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[z] + x;
    props = TerrainModifiers_FindCellOverride(cell, 1);
    if (props)
        props = (u8 *)&((struct TerrainCellOverride *)props)->cell;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 index = cell;
        props = (u8 *)&((struct TerrainGrid *)map->cells)->cells[index];
    }
    surface = *props >> 4;
    height = (*props & 15) << 8;
    switch (surface) {
        case 1:
            height -= (u8)out->z;
            break;
        case 2:
            height -= (u8)out->x;
            break;
    }
    out->y = height;
    return point;
}

s32 Path_SnapToNearestPoint(struct Pos16 *out, struct Pos16 *ref, u32 pathId, s32 unusedDistance)
{
    u32 minDist;
    s32 best = -1;
    struct SceneActorRouteDefinition *def = Path_GetDefinition(pathId);
    s32 i;
    struct {
        struct SceneActorRoutePoint *data;
    } L;
    struct Pos16 pos;
    if (!def || !def->pointCount) {
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
    for (i = 0; i < def->pointCount; i++) {
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

s32 Path_SnapToFarthestPoint(struct Pos16 *out, struct Pos16 *ref, u32 pathId, s32 unusedDistance)
{
    u32 maxDist;
    s32 best = -1;
    struct SceneActorRouteDefinition *def = Path_GetDefinition(pathId);
    s32 i;
    struct {
        struct SceneActorRoutePoint *data;
    } L;
    struct Pos16 pos;
    if (!def || !def->pointCount) {
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
    maxDist = 0;
    for (i = 0; i < def->pointCount; i++) {
        s32 dx, dz;
        u32 d;
        Path_GetPointPosition(&pos, L.data, i);
        dx = (s16)ref->x - (s16)pos.x;
        dz = (s16)ref->z - (s16)pos.z;
        d = dx * dx + dz * dz;
        if (d >= maxDist) {
            best = i;
            maxDist = d;
            out->x = pos.x;
            out->z = pos.z;
        }
    }
    Pos16_SnapToTerrain(out);
    return best;
}

static inline s32 CellIndex(struct ContactPosition *p)
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
        return (u8 *)&((struct TerrainCellOverride *)p)->cell;
    return (u8 *)&((struct TerrainGrid *)gTerrainMap->cells)->cells[i];
}

static inline s32 Height(struct ContactPosition *p)
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

s32 Path_FindNearestDistantPoint(struct ContactPosition *out, struct ContactPosition *from, u32 id,
                                 u32 minArg)
{
    u32 minimum = minArg;
    struct ContactPosition point;
    s32 chosen = -1;
    struct SceneActorRouteDefinition *def = Path_GetDefinition(id);
    void *data;
    u32 best;
    s32 i;
    if (!def || !def->pointCount) {
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
    Path_GetPointPosition((struct Pos16 *)&point, data, 0);
    out->x = point.x;
    out->z = point.z;
    best = chosen;
    for (i = 0; i < def->pointCount; i++) {
        s32 x, z;
        u32 distance;
        Path_GetPointPosition((struct Pos16 *)&point, data, i);
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

s32 Path_SnapToRandomFarPoint(struct Pos16 *out, struct Pos16 *ref, u32 pathId, u32 minDist)
{
    struct SceneActorRouteDefinition *def = Path_GetDefinition(pathId);
    s32 start, i;
    struct Pos16 pos;
    struct {
        s32 idx;
    } L;
    if (!def || !def->pointCount) {
        out->x = 0;
        out->y = 0;
        out->z = 0;
        return -1;
    }
    {
        u32 r = (gRandomIndex + 1) & 0x3FF;
        gRandomIndex = r;
        start = Mod(*(const u16 *)((u8 *)gRandomTable + r * 2), def->pointCount);
    }
    for (i = start; i < start + def->pointCount; i++) {
        s32 dx, dz;
        L.idx = Mod(i, def->pointCount);
        Path_GetPointPositionById(&pos, pathId, L.idx);
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

// The random table (gRandomTable) is addressed by its numeric address here; the

// symbol form changes reload order and register choice in the loop.

static inline u8 PosInBox(struct ContactPosition *p, struct ContactPosition *lo,
                          struct ContactPosition *hi)
{
    if (!(p->x >= lo->x && p->x <= hi->x))
        return FALSE;
    if (p->z >= lo->z && p->z <= hi->z)
        return TRUE;
    return FALSE;
}

s32 Path_SnapToRandomPointInBox(struct Pos16 *out, struct Pos16 *lo, struct Pos16 *hi, u32 pathId)
{
    s32 best = -1;
    struct SceneActorRouteDefinition *def = Path_GetDefinition(pathId);
    struct SceneActorRoutePoint *data;
    s32 start, i;
    struct Pos16 pos;
    if (!def || !def->pointCount) {
        out->x = 0;
        out->y = 0;
        out->z = 0;
        return best;
    }
    data = Path_GetPointData(def);
    if (!data) {
        out->x = data;
        out->y = data;
        out->z = data;
        return best;
    }
    {
        u32 r = (gRandomIndex + 1) & 0x3FF;
        gRandomIndex = r;
        start = Mod(*(const u16 *)((u8 *)0x0203B400 + r * 2), def->pointCount);
    }
    Path_GetPointPosition(&pos, data, 0);
    out->x = pos.x;
    out->z = pos.z;
    for (i = start; i < start + def->pointCount; i++) {
        s32 idx = Mod(i, def->pointCount);
        Path_GetPointPosition(&pos, data, idx);
        if (PosInBox((struct ContactPosition *)&pos, (struct ContactPosition *)lo,
                     (struct ContactPosition *)hi)) {
            best = idx;
            out->x = pos.x;
            out->z = pos.z;
        }
    }
    Pos16_SnapToTerrain(out);
    return best;
}

// Path dispatcher: snap `out` onto a path point chosen by `mode`, falling back to a

// random point when the chosen helper finds none.

s32 Path_SelectPoint(struct Pos16 *out, struct Pos16 *ref, u32 mode, u32 pathId, s32 dist,
                     struct Pos16 *lo, struct Pos16 *hi)
{
    s32 d = dist;
    s32 result = -1;
    d *= d;
    switch (mode) {
        case 0:
            result = Path_PickRandomPoint(out, pathId);
            break;
        case 1:
            result = Path_SnapToNearestPoint(out, ref, pathId, d);
            break;
        case 2:
            result = Path_SnapToFarthestPoint(out, ref, pathId, d);
            break;
        case 3:
            result = Path_FindNearestDistantPoint((struct ContactPosition *)out,
                                                  (struct ContactPosition *)ref, pathId, d);
            break;
        case 4:
            result = Path_SnapToRandomFarPoint(out, ref, pathId, d);
            break;
        case 5:
            result = Path_SnapToRandomPointInBox(out, lo, hi, pathId);
            break;
    }
    if (result < 0)
        result = Path_PickRandomPoint(out, pathId);
    return result;
}
