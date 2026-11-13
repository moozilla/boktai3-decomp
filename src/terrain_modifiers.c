#define Terrain_IsSegmentBelowHeightLimit  sub_0821D4F0
#define Terrain_StepTowardCellBoundary     sub_0821D178
#define Terrain_MovePoint                  sub_0821C884
#define Terrain_AdjustBlockedMovement      sub_0821C254
#define Terrain_AreCellsConnected          sub_0821BF5C
#define Terrain_GetHeightDifference        sub_0821BE3C
#define Terrain_GetCellFlagsAt             sub_0821BDD8
#define Terrain_GetCardinalDirectionToCell sub_0821BCE4
#include "terrain_collision.h"
#include "engine_globals.h"
#include "engine.h"
#include "ram.h"
#include "game_state.h"
#include "terrain_cell_probe.h"
#include "terrain_probe.h"
#include "contact_position.h"

void ResourceRegistry_RegisterResource(u32, void *, u32);

s32 Terrain_ResetMap(void);

void Terrain_Init(void)
{
    void *p = Heap_Alloc(0xE24);
    Memory_Clear(p, 0xE24);
    ResourceRegistry_RegisterResource(0x56C2, p, 1);
    gTerrainMap = p;
    Terrain_ResetMap();
}

#define Terrain_ClearTrap Trap_Init

void Trap_Init(void *);

s32 Terrain_ResetMap(void)
{
    s32 i;
    struct EngineMap *map = gTerrainMap;
    if (!map)
        return 0;
    map->cells = 0;
    map->resource8 = 0;
    map->boxes = 0;
    map->routes = 0;
    map->boxList = 0;
    gTerrainTrapSerial = 0;
    map->trapCount = 0;
    for (i = 0; i < 64; i++) {
        Trap_Init(&gTerrainMap->traps[i]);
        gTerrainMap->trapTops[i] = 0;
    }
    return 1;
}

void Terrain_SetNeighborOffsets(void)
{
    struct EngineMap *a = gTerrainMap;
    struct TerrainGrid *grid = (struct TerrainGrid *)a->cells;
    a->neighborOffsets[0] = -(u16)grid->width;
    a->neighborOffsets[1] = 1;
    a->neighborOffsets[2] = (u16)grid->width;
    a->neighborOffsets[3] = 0xFFFF;
}

void Terrain_BuildRowOffsets(void)
{
    s32 width = ((struct TerrainGrid *)gTerrainMap->cells)->width;
    u16 *row = gTerrainMap->rows;
    s32 offset = 0;
    s32 i = 0;
    for (; i < ((struct TerrainGrid *)gTerrainMap->cells)->height; i++) {
        row[i] = offset;
        offset += width;
    }
}

void Terrain_UpdateSize(void)
{
    gTerrainWidth = ((struct TerrainGrid *)gTerrainMap->cells)->width;
    gTerrainHeight = ((struct TerrainGrid *)gTerrainMap->cells)->height;
}

void Terrain_SetNeighborOffsets(void);

void Terrain_BuildRowOffsets(void);

void Terrain_UpdateSize(void);

u32 Terrain_LoadGridResource(u16 v)
{
    gTerrainMap->cells = ScriptProc_Start(0xAE1B, v);
    Terrain_SetNeighborOffsets();
    Terrain_BuildRowOffsets();
    Terrain_UpdateSize();
    return 0;
}

void Terrain_SetGrid(void *v)
{
    gTerrainMap->cells = v;
    Terrain_SetNeighborOffsets();
    Terrain_BuildRowOffsets();
    Terrain_UpdateSize();
}

void CellObject_Init(struct SurfaceTrack *obj, u16 *point)
{
    u32 cell;
    u8 *def;
    s32 x, z;
    u8 lo;
    u8 hi;
    u16 value;
    struct EngineMap *map = gTerrainMap;

    if (map->cells) {
        u8 *cells = map->cells;
        u32 off;
        x = (s16)point[0] >> 8;
        z = (s16)point[2] >> 8;
        if (x < 0 || z < 0 || x >= gTerrainWidth || z >= gTerrainHeight)
            cell = 0;
        else
            cell = map->rows[z] + x;
        obj->cell = cell;
        obj->previousCell = cell;
        off = obj->cell;
        def = (u8 *)&((struct TerrainGrid *)cells)->cells[off];
        lo = *def & 15;
        obj->height = lo;
        hi = *def >> 4;
        obj->shape = hi;
        value = ((struct TerrainCell *)def)->flags;
        obj->flags = value;
        obj->previousHeight = lo;
        obj->previousShape = hi;
        obj->previousFlags = value;
    }
}

s32 Terrain_GetCardinalDirectionToCell(s32 start, s32 target)
{
    s32 i, j;
    for (i = 0; i < 4; i++) {
        s32 off = gTerrainMap->neighborOffsets[i & 3];
        s32 pos = start;
        for (j = 0; j < 4; j++) {
            pos += off;
            if (pos == target)
                return (192 + i * 64) & 0xff;
        }
    }
    return -1;
}

u8 *TerrainModifiers_FindCellOverride(s32, s32);

u16 Terrain_GetHeightAt(u16 *point)
{
    s32 x, z;
    u32 cell;
    u8 *definition;
    u32 type;
    s32 height;

    x = (s16)point[0] >> 8;
    z = (s16)point[2] >> 8;
    if (x < 0 || z < 0 || x >= gTerrainWidth || z >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[z] + x;
    definition = TerrainModifiers_FindCellOverride(cell, 1);
    if (definition)
        definition = (u8 *)&((struct TerrainCellOverride *)definition)->cell;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 index = cell;
        definition = (u8 *)&((struct TerrainGrid *)map->cells)->cells[index];
    }
    type = *definition >> 4;
    height = (*definition & 15) << 8;
    switch (type) {
        case 1:
            height -= (u8)point[2];
            break;
        case 2:
            height -= (u8)point[0];
            break;
    }
    return height;
}

u16 Terrain_GetCellFlagsAt(u16 *pos)
{
    struct EngineMap *map = gTerrainMap;
    u8 *cells;
    s32 x, z, index;
    if (map->cells == NULL)
        return 0;
    cells = map->cells;
    x = (s16)pos[0] >> 8;
    z = (s16)pos[2] >> 8;
    if (x < 0 || z < 0 || x >= gTerrainWidth || z >= gTerrainHeight)
        index = 0;
    else
        index = map->rows[z] + x;
    /* Keep the indexed byte displacement first for the original Thumb operand order. */
    return ((struct TerrainGrid *)(index * sizeof(struct TerrainCell) + (u32)cells))
        ->cells[0]
        .flags;
}

static inline s32 Terrain_GetCellIndex(struct ContactPosition *pos)
{
    s32 x = pos->x >> 8, z = pos->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

static inline s32 Terrain_SampleHeight(struct ContactPosition *position)
{
    s32 index = Terrain_GetCellIndex(position), height;
    u8 *cell = TerrainModifiers_FindCellOverride(index, 1);
    u32 kind;
    if (cell)
        cell = (u8 *)&((struct TerrainCellOverride *)cell)->cell;
    else
        cell = (u8 *)&((struct TerrainGrid *)gTerrainMap->cells)->cells[index];
    kind = *cell >> 4;
    height = (*cell & 15) << 8;
    switch (kind) {
        case 1:
            height -= (u8)position->z;
            break;
        case 2:
            height -= (u8)position->x;
            break;
    }
    return height;
}

u16 Terrain_GetHeightDifference(struct ContactPosition *a, struct ContactPosition *b)
{
    s32 ha = Terrain_SampleHeight(a);
    s32 d = ha - Terrain_SampleHeight(b);
    if (d < 0)
        d = -d;
    return d;
}

u16 Terrain_GetHeightDifference(struct ContactPosition *a, struct ContactPosition *b);

#define CELL_AT(cell, index)                                                                       \
    {                                                                                              \
        u8 *found = TerrainModifiers_FindCellOverride(index, 1);                                   \
        if (found)                                                                                 \
            cell = &((struct TerrainCellOverride *)found)->cell;                                   \
        else                                                                                       \
            cell = &((struct TerrainGrid *)gTerrainMap->cells)->cells[index];                      \
    }

static inline void TerrainCell_MoveToPreviousRow(s32 *cellIndex)
{
    *cellIndex -= gTerrainWidth;
}

/* Check whether the two terrain cells connect along their stair axes. */

u32 Terrain_AreCellsConnected(struct ContactPosition *from, struct ContactPosition *to, u8 kind)
{
    u16 toIndex;
    u16 fromIndex;
    struct TerrainCell *toCell;
    struct TerrainCell *fromCell;
    struct TerrainCell *cell;
    u16 toFlags;
    u32 mask;
    s32 limit;
    s32 cellDelta;
    s32 neighborIndex;
    u32 fromKind;
    u32 toKind;
    toIndex = Terrain_GetCellIndex(to);
    {
        u8 *found = TerrainModifiers_FindCellOverride(toIndex, 1);
        if (found)
            toCell = &((struct TerrainCellOverride *)found)->cell;
        else
            toCell = &((struct TerrainGrid *)gTerrainMap->cells)->cells[toIndex];
    };
    toFlags = toCell->flags;
    if (kind == 4) {
        mask = 2;
        limit = 0x80;
    } else {
        mask = 1;
        limit = 100;
    }
    if (toFlags & mask)
        goto blocked;
    if (Terrain_GetHeightDifference(from, to) > limit)
        goto blocked;
    fromIndex = Terrain_GetCellIndex(from);
    {
        u8 *found = TerrainModifiers_FindCellOverride(fromIndex, 1);
        if (found)
            fromCell = &((struct TerrainCellOverride *)found)->cell;
        else
            fromCell = &((struct TerrainGrid *)gTerrainMap->cells)->cells[fromIndex];
    };
    fromKind = fromCell->shapeAndHeight >> 4;
    toKind = toCell->shapeAndHeight >> 4;
    cellDelta = toIndex - fromIndex;
    if (fromKind == 1) {
        if ((cellDelta != 1) && (cellDelta != (-1)))
            return 1;
        if ((gGameState->flags868 & 0x10) && (toFlags & 0x80))
            goto blocked;
        if (toKind != 0)
            return 1;
        if ((to->z & 0xFF) <= 0x7F) {
            neighborIndex = toIndex;
            TerrainCell_MoveToPreviousRow(&neighborIndex);
            CELL_AT(cell, neighborIndex);
        } else {
            neighborIndex = toIndex + gTerrainWidth;
            CELL_AT(cell, neighborIndex);
        }
        {
            u32 cellHeight, referenceHeight;
            if (cell->shapeAndHeight >> 4)
                goto blocked;
            cellHeight = cell->shapeAndHeight & 15;
            referenceHeight = toCell->shapeAndHeight & 15;
            if (cellHeight != referenceHeight)
                goto blocked;
            if (!(cell->flags & mask))
                goto success;
            goto blocked;
        }
    } else if (fromKind == 2) {
        if ((cellDelta != gTerrainWidth) && (cellDelta != (-gTerrainWidth)))
            return 1;
        if ((gGameState->flags868 & 0x10) && (toFlags & 0x80))
            goto blocked;
        if (toKind != 0)
            return 1;
        if ((to->x & 0xFF) <= 0x7F) {
            neighborIndex = toIndex - 1;
            CELL_AT(cell, neighborIndex);
        } else {
            neighborIndex = toIndex + 1;
            CELL_AT(cell, neighborIndex);
        }
        {
            u32 cellHeight, referenceHeight;
            if (cell->shapeAndHeight >> 4)
                goto blocked;
            cellHeight = cell->shapeAndHeight & 15;
            referenceHeight = toCell->shapeAndHeight & 15;
            if (cellHeight != referenceHeight)
                goto blocked;
            if (!(cell->flags & mask))
                goto success;
            goto blocked;
        }
    } else {
        neighborIndex = fromIndex;
        if ((cellDelta == gTerrainWidth) || (cellDelta == (-gTerrainWidth))) {
            if (toKind != 2)
                return 1;
            if ((to->x & 0xFF) <= 0x7F) {
                neighborIndex--;
                goto lookupNeighbor;
            } else {
                neighborIndex++;
                goto lookupNeighbor;
            }
        } else if ((cellDelta == 1) || (cellDelta == (-1))) {
            if (toKind != 1)
                return 1;
            if ((to->z & 0xFF) <= 0x7F) {
                TerrainCell_MoveToPreviousRow(&neighborIndex);
            lookupNeighbor:
                CELL_AT(cell, neighborIndex);
            } else {
                neighborIndex += gTerrainWidth;
                CELL_AT(cell, neighborIndex);
            }
        } else
            return 1;
        {
            u32 cellHeight, referenceHeight;
            if (cell->shapeAndHeight >> 4)
                goto blocked;
            cellHeight = cell->shapeAndHeight & 15;
            referenceHeight = fromCell->shapeAndHeight & 15;
            if (cellHeight != referenceHeight)
                goto blocked;
            if (!(cell->flags & mask))
                goto success;
            goto blocked;
        }
    }
blocked:
    return 0;
success:
    return 1;
}

s32 Mod(s32, s32);

s32 Div(s32, s32);

u32 Terrain_AreCellsConnected(struct ContactPosition *from, struct ContactPosition *to, u8 kind);

#define WIDTH (grid->width)

#define LO(v) (((v) << 8) + 1)

#define HI(v) ((((v) + 1) << 8) - 1)

static inline u32 AcceptPosition(struct ContactPosition *destination,
                                 const struct ContactPosition *candidate)
{
    *destination = *candidate;
    return 1;
}

u32 Terrain_AdjustBlockedMovement(struct SurfaceTrack *t, struct ContactPosition *from,
                                  struct ContactPosition *destination, u32 kindArg)
{
    struct ContactPosition *to = destination;
    u8 kind = kindArg;
    struct ContactPosition p;
    struct TerrainGrid *grid;
    u16 idx;
    s32 diff;
    if (gTerrainMap->cells == 0)
        return 0;
    grid = (struct TerrainGrid *)gTerrainMap->cells;
    {
        s32 x = to->x >> 8, z = to->z >> 8;
        s32 index;
        if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
            index = 0;
        else
            index = gTerrainMap->rows[z] + x;
        idx = index;
    }
    if ((t->cell == idx) || Terrain_AreCellsConnected(from, to, kind))
        return 0;
    diff = idx - t->cell;
    if (diff <= ((-(grid->width)) + 1)) {
        s32 delta = diff + 1;
        diff = delta + grid->width;
    } else if (diff == (-1))
        diff = 3;
    else if (diff == 1)
        diff = 4;
    else if (diff >= ((grid->width) - 1))
        diff = diff + (6 - (grid->width));
    switch ((u32)diff) {
        case 0:
            if (t->previousCell == (idx + 1)) {
                p = *to;
                p.x = (Mod(t->cell, grid->width) << 8) + 1;
                if (Terrain_AreCellsConnected(from, &p, kind))
                    return AcceptPosition(to, &p);
                p = *to;
                p.z = (Div(t->cell, grid->width) << 8) + 1;
                if (Terrain_AreCellsConnected(from, &p, kind))
                    return AcceptPosition(to, &p);
                to->x = (Mod(t->cell, grid->width) << 8) + 1;
                to->z = (Div(t->cell, grid->width) << 8) + 1;
                break;
            } else {
                p = *to;
                p.z = (Div(t->cell, grid->width) << 8) + 1;
                if (Terrain_AreCellsConnected(from, &p, kind))
                    return AcceptPosition(to, &p);
                p = *to;
                p.x = (Mod(t->cell, grid->width) << 8) + 1;
                if (Terrain_AreCellsConnected(from, &p, kind))
                    return AcceptPosition(to, &p);
                to->x = (Mod(t->cell, grid->width) << 8) + 1;
                to->z = (Div(t->cell, grid->width) << 8) + 1;
                break;
            }
            break;

        case 1:
            to->z = (Div(t->cell, grid->width) << 8) + 1;
            break;

        case 2:
            if (t->previousCell == (idx - 1)) {
                p = *to;
                p.x = ((Mod(t->cell, grid->width) + 1) << 8) - 1;
                if (Terrain_AreCellsConnected(from, &p, kind))
                    return AcceptPosition(to, &p);
                p = *to;
                p.z = (Div(t->cell, grid->width) << 8) + 1;
                if (Terrain_AreCellsConnected(from, &p, kind))
                    return AcceptPosition(to, &p);
                to->x = ((Mod(t->cell, grid->width) + 1) << 8) - 1;
                to->z = (Div(t->cell, grid->width) << 8) + 1;
                break;
            } else {
                p = *to;
                p.z = (Div(t->cell, grid->width) << 8) + 1;
                if (Terrain_AreCellsConnected(from, &p, kind))
                    return AcceptPosition(to, &p);
                p = *to;
                p.x = ((Mod(t->cell, grid->width) + 1) << 8) - 1;
                if (Terrain_AreCellsConnected(from, &p, kind))
                    return AcceptPosition(to, &p);
                to->x = ((Mod(t->cell, grid->width) + 1) << 8) - 1;
                to->z = (Div(t->cell, grid->width) << 8) + 1;
                break;
            }
            break;

        case 3:
            to->x = (Mod(t->cell, grid->width) << 8) + 1;
            break;

        case 4:
            to->x = ((Mod(t->cell, grid->width) + 1) << 8) - 1;
            break;

        case 5:
            if (t->previousCell == (idx + 1)) {
                p = *to;
                p.x = (Mod(t->cell, grid->width) << 8) + 1;
                if (Terrain_AreCellsConnected(from, &p, kind))
                    goto copy;
                p = *to;
                p.z = ((Div(t->cell, grid->width) + 1) << 8) - 1;
                if (Terrain_AreCellsConnected(from, &p, kind))
                    goto copy;
                to->x = (Mod(t->cell, grid->width) << 8) + 1;
                to->z = ((Div(t->cell, grid->width) + 1) << 8) - 1;
                break;
            } else {
                p = *to;
                p.z = ((Div(t->cell, grid->width) + 1) << 8) - 1;
                if (Terrain_AreCellsConnected(from, &p, kind))
                    goto copy;
                p = *to;
                p.x = (Mod(t->cell, grid->width) << 8) + 1;
                if (Terrain_AreCellsConnected(from, &p, kind))
                    goto copy;
                to->x = (Mod(t->cell, grid->width) << 8) + 1;
                to->z = ((Div(t->cell, grid->width) + 1) << 8) - 1;
                break;
            }
            break;

        case 6:
            to->z = ((Div(t->cell, grid->width) + 1) << 8) - 1;
            break;

        case 7:
            if (t->previousCell == (idx - 1)) {
                p = *to;
                p.x = ((Mod(t->cell, grid->width) + 1) << 8) - 1;
                if (Terrain_AreCellsConnected(from, &p, kind))
                    goto copy;
                p = *to;
                p.z = ((Div(t->cell, grid->width) + 1) << 8) - 1;
                if (Terrain_AreCellsConnected(from, &p, kind))
                    goto copy;
            } else {
                p = *to;
                p.z = ((Div(t->cell, grid->width) + 1) << 8) - 1;
                if (Terrain_AreCellsConnected(from, &p, kind))
                    goto copy;
                p = *to;
                p.x = ((Mod(t->cell, grid->width) + 1) << 8) - 1;
                if (Terrain_AreCellsConnected(from, &p, kind)) {
                copy:
                    *to = p;

                    break;
                }
            }
            to->x = ((Mod(t->cell, grid->width) + 1) << 8) - 1;
            to->z = ((Div(t->cell, grid->width) + 1) << 8) - 1;
            break;
    }

    return 1;
}

void Terrain_SampleSurface(struct SurfaceTrack *t, struct Pos16 *out, struct Pos16 *in)
{
    if (gTerrainMap->cells) {
        u32 cell = (u16)cellIndexAt(in);
        u8 *props = cellProps(cell);
        if (t->cell != cell) {
            t->previousCell = t->cell;
            t->previousShape = t->shape;
            t->previousHeight = t->height;
            t->previousFlags = t->flags;
            t->cell = cell;
            t->shape = props[0] >> 4;
            t->height = props[0] & 0xf;
            t->flags = ((struct TerrainCell *)props)->flags;
        }
        *(struct Pos16 *)out = *in;
        {
            u32 c2 = cellIndexAt(in);
            u8 *p2 = cellProps(c2);
            u32 type = *p2 >> 4;
            u32 h = (*p2 & 0xf) << 8;
            switch (type) {
                case 1:
                    h -= (u8)in->z;
                    break;
                case 2:
                    h -= (u8)in->x;
                    break;
            }
            out->y = h;
        }
    }
}

u32 Terrain_AdjustBlockedMovement(struct SurfaceTrack *, struct ContactPosition *,
                                  struct ContactPosition *, u32);

void Terrain_SampleSurface(struct SurfaceTrack *, struct Pos16 *, struct Pos16 *);

void Terrain_MovePoint(struct SurfaceTrack *out, u16 *pos, u16 *delta, u32 unused, u32 unused2,
                       u8 flag)
{
    struct TerrainProbe probe;
    probe.x = pos[0] + delta[0];
    probe.y = pos[1] + delta[1];
    probe.z = pos[2] + delta[2];
    probe.w = 0x10;

    out->blocked = Terrain_AdjustBlockedMovement(out, pos, &probe, flag);
    Terrain_SampleSurface(out, pos, &probe);
}

u32 Terrain_AreCellsConnected(struct ContactPosition *, struct ContactPosition *, u8);

#define HIGH_EDGE(n) ((((n) + 1) << 8) - 1)

#define LOW_EDGE(n) (((n) << 8) + 1)

#define OFFSET(v, step, sign) ((sign) > 0 ? (v) + (step) : (v) - (step))

#define UNOFFSET(v, step, sign) ((sign) > 0 ? (v) - (step) : (v) + (step))

#define BOUND(n, sign) ((sign) > 0 ? HIGH_EDGE(n) : LOW_EDGE(n))

#define SET_FOOTPRINT_CORNER(xSign, zSign)                                                         \
    corner = destination;                                                                          \
    corner.x = OFFSET(corner.x, stepX, xSign);                                                     \
    corner.z = OFFSET(corner.z, stepZ, zSign);

#define SAMPLE_ADJUSTED_CORNER(xSign, zSign)                                                       \
    corner.x = UNOFFSET(corner.x, stepX, xSign);                                                   \
    corner.z = UNOFFSET(corner.z, stepZ, zSign);                                                   \
    Terrain_SampleSurface(track, pos, &corner);

#define TEST_FOOTPRINT_AXES(xSign, zSign)                                                          \
    probeX = *(struct TerrainProbe *)pos;                                                          \
    probeX.x += OFFSET(delta->x, stepX, xSign);                                                    \
    openX = Terrain_AreCellsConnected(pos, &probeX, flag);                                         \
    probeZ = *(struct TerrainProbe *)pos;                                                          \
    probeZ.z += OFFSET(delta->z, stepZ, zSign);                                                    \
    openZ = Terrain_AreCellsConnected(pos, &probeZ, flag);

#define SAMPLE_BLOCKED_X(xSign)                                                                    \
    Terrain_AdjustBlockedMovement(track, pos, &probeX, flag);                                      \
    probeX.x = UNOFFSET(probeX.x, stepX, xSign);                                                   \
    probeX.z += delta->z;                                                                          \
    Terrain_SampleSurface(track, pos, &probeX);

#define SAMPLE_BLOCKED_Z(zSign)                                                                    \
    Terrain_AdjustBlockedMovement(track, pos, &probeZ, flag);                                      \
    probeZ.x += delta->x;                                                                          \
    probeZ.z = UNOFFSET(probeZ.z, stepZ, zSign);                                                   \
    Terrain_SampleSurface(track, pos, &probeZ);

#define PROBE_CARDINAL(x1, z1, x2, z2)                                                             \
    {                                                                                              \
        SET_FOOTPRINT_CORNER(x1, z1);                                                              \
        if (Terrain_AdjustBlockedMovement(track, pos, &corner, flag)) {                            \
            SAMPLE_ADJUSTED_CORNER(x1, z1);                                                        \
            track->blocked = 1;                                                                    \
            return;                                                                                \
        } else {                                                                                   \
            SET_FOOTPRINT_CORNER(x2, z2);                                                          \
            if (Terrain_AdjustBlockedMovement(track, pos, &corner, flag)) {                        \
                SAMPLE_ADJUSTED_CORNER(x2, z2);                                                    \
                track->blocked = 1;                                                                \
                return;                                                                            \
            } else {                                                                               \
                Terrain_SampleSurface(track, pos, &destination);                                   \
            }                                                                                      \
        }                                                                                          \
    }

/* A blocked X axis finishes through the exit for its X direction. */

#define PROBE_DIAGONAL(xSign, zSign, finished, xBlocked)                                           \
    {                                                                                              \
        SET_FOOTPRINT_CORNER(xSign, zSign);                                                        \
        if (Terrain_AdjustBlockedMovement(track, pos, &corner, flag)) {                            \
            SAMPLE_ADJUSTED_CORNER(xSign, zSign);                                                  \
            track->blocked = 1;                                                                    \
            return;                                                                                \
        } else {                                                                                   \
            struct TerrainProbe probeX, probeZ;                                                    \
            u32 openX, openZ;                                                                      \
            TEST_FOOTPRINT_AXES(xSign, zSign);                                                     \
            if (openX && openZ) {                                                                  \
                Terrain_SampleSurface(track, pos, &destination);                                   \
            } else if (!openX && !openZ) {                                                         \
                corner.x = BOUND(Mod(track->cell, gTerrainWidth), xSign);                          \
                corner.z = BOUND(Div(track->cell, gTerrainWidth), zSign);                          \
                SAMPLE_ADJUSTED_CORNER(xSign, zSign);                                              \
                goto finished;                                                                     \
            } else if (!openX) {                                                                   \
                SAMPLE_BLOCKED_X(xSign);                                                           \
                goto xBlocked;                                                                     \
            } else {                                                                               \
                SAMPLE_BLOCKED_Z(zSign);                                                           \
            finished:                                                                              \
                track->blocked = 1;                                                                \
                return;                                                                            \
            }                                                                                      \
        }                                                                                          \
    }

void Terrain_MoveFootprint(struct SurfaceTrack *track, struct ContactPosition *pos,
                           struct ContactPosition *delta, u16 stepX, u16 stepZ, u8 flag)
{
    u32 mode = 0;
    u8 clear;
    s32 tileX, tileZ, cell;
    struct TerrainProbe destination, corner;
    if (delta->x > 0)
        mode = 0x10;
    else if (delta->x < 0)
        mode = 0x20;
    if (delta->z > 0)
        mode |= 1;
    else if (delta->z < 0)
        mode |= 2;
    tileX = (s8)(pos->x >> 8);
    tileZ = (s8)(pos->z >> 8);
    if (tileX < 0 || tileZ < 0 || (u32)tileX >= gTerrainWidth || (u32)tileZ >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[tileZ] + tileX;
    clear = 0;
    track->cell = cell;
    destination.x = pos->x + delta->x;
    destination.y = pos->y + delta->y;
    destination.z = pos->z + delta->z;
    destination.w = 0x10;
    track->blocked = clear;
    switch (mode) {
        case 0:
            Terrain_SampleSurface(track, pos, &destination);
            break;
        case 0x10:
            PROBE_CARDINAL(1, 1, 1, -1);
            break;
        case 0x20:
            PROBE_CARDINAL(-1, 1, -1, -1);
            break;
        case 0x01:
            PROBE_CARDINAL(1, 1, -1, 1);
            break;
        case 0x02:
            PROBE_CARDINAL(1, -1, -1, -1);
            break;
        case 0x11:
            PROBE_DIAGONAL(1, 1, hitPosXPosZ, hitPosXNegZ);
            break;
        case 0x21:
            PROBE_DIAGONAL(-1, 1, hitNegXPosZ, hitNegXNegZ);
            break;
        case 0x12:
            PROBE_DIAGONAL(1, -1, hitPosXNegZ, hitPosXNegZ);
            break;
        case 0x22:
            PROBE_DIAGONAL(-1, -1, hitNegXNegZ, hitNegXNegZ);
            break;
    }
    return;
}

s32 Terrain_StepTowardCellBoundary(struct ContactPosition *out, struct ContactPosition *from,
                                   struct ContactPosition *to, s32 previousStep)
{
    struct ContactPosition p;
    s32 x0, z0, x1, z1;
    s32 dx, dz;
    s32 fx, fz, rx, rz;
    u32 flags;

    dx = to->x - from->x;
    dz = to->z - from->z;
    p.y = from->y;
    fx = (u8)from->x;
    fz = (u8)from->z;
    rx = 0x100 - fx;
    rz = 0x100 - fz;
    x0 = from->x & 0xFF00;
    z0 = from->z & 0xFF00;
    x1 = x0 + 0x100;
    z1 = z0 + 0x100;
    if (to->x >= x0 && to->x <= x1 && to->z >= z0 && to->z <= z1) {
        *out = *to;
        return 1;
    }
    if (dx == 0) {
        if (dz == 0) {
            *out = *to;
            return 1;
        }
        if (dz > 0)
            p.z = z1;
        else
            p.z = z0;
        p.x = from->x;
        *out = p;
        return dz > 0 ? 2 : 3;
    }
    if (dz == 0) {
        if (dx > 0)
            p.x = x1;
        else
            p.x = x0;
        p.z = from->z;
        *out = p;
        return dx > 0 ? 5 : 4;
    }
    flags = 0x100;
    if (dx > 0)
        flags = 0x1000;
    if (dz > 0)
        flags |= 0x10;
    else
        flags |= 1;
    if (flags & 0x10) {
        p.x = from->x + Div(rz * dx, dz < 0 ? -dz : dz);
        p.z = z1;
        if (p.x >= x0 && p.x <= x1) {
            if ((p.x & 0xFF00) > (from->x & 0xFF00))
                p.x = from->x | 0xFF;
            *out = p;
            return 3;
        }
    }
    if (flags & 1) {
        p.x = from->x + Div(fz * dx, dz < 0 ? -dz : dz);
        p.z = z0 - 1;
        if (p.x >= x0 && p.x <= x1) {
            if ((p.x & 0xFF00) > (from->x & 0xFF00))
                p.x = from->x | 0xFF;
            *out = p;
            return 2;
        }
    }
    if (flags & 0x100) {
        p.x = x0 - 1;
        p.z = from->z + Div(dz * fx, dx < 0 ? -dx : dx);
        if (p.z >= z0 && p.z <= z1) {
            if ((p.z & 0xFF00) > (from->z & 0xFF00))
                p.z = from->z | 0xFF;
            *out = p;
            return 4;
        }
    }
    if (flags & 0x1000) {
        p.x = x1;
        p.z = from->z + Div(dz * rx, dx < 0 ? -dx : dx);
        if (p.z >= z0 && p.z <= z1) {
            if ((p.z & 0xFF00) > (from->z & 0xFF00))
                p.z = from->z | 0xFF;
            *out = p;
            return 5;
        }
    }
    return 0;
}

s32 Terrain_StepTowardCellBoundary(struct ContactPosition *out, struct ContactPosition *from,
                                   struct ContactPosition *target, s32 previousStep);

static inline u32 cellHeightAt(struct ContactPosition *pos, s32 cell)
{
    u8 *props = cellProps(cell);
    u32 type = *props >> 4;
    u32 h = (*props & 0xf) << 8;
    switch (type) {
        case 1:
            h -= (u8)pos->z;
            break;
        case 2:
            h -= (u8)pos->x;
            break;
    }
    return h;
}

// Walks up to `steps` path steps from `start`; returns 1 when the walk finishes

// (Terrain_StepTowardCellBoundary returns 1) before the terrain rises `maxRise` above start->y, else 0.

s32 Terrain_IsSegmentBelowHeightLimit(struct ContactPosition *start, struct ContactPosition *target,
                                      s32 steps, s32 maxRise)
{
    struct ContactPosition cur, next;
    s32 i, cell, stepResult;

    if (gTerrainMap->cells == NULL)
        return 0;
    stepResult = 0;
    cur = *start;
    cell = cellIndexAt((struct Pos16 *)&cur);
    for (i = 0; i < steps; i++) {
        struct ContactPosition *nextPos = &next;
        stepResult = Terrain_StepTowardCellBoundary(nextPos, &cur, target, stepResult);
        if (stepResult > 1)
            cell = cellIndexAt((struct Pos16 *)nextPos);
        if ((s32)(cellHeightAt(&next, cell) - start->y) >= maxRise)
            return 0;
        if (stepResult == 1)
            return 1;
        cur = next;
    }
    return 0;
}

void TerrainModifiers_InitCell(struct TerrainCellOverride *r, u32 a, u32 b, u32 c, u32 d, u32 e)
{
    r->index = a;
    r->flags = 0;
    r->cell.shapeAndHeight = (b << 4) | c;
    r->cell.attribute = d;
    r->cell.flags = e;
}

u8 *TerrainModifiers_FindCellOverride(s32 k, s32 m)
{
    struct TerrainCellOverride *best;
    struct TerrainCellOverride *n = gTerrainMap->boxList;
    best = 0;
    for (; n; n = n->next) {
        if ((n->flags & m) == 0 && n->index == k) {
            if (best == 0 || (best->cell.shapeAndHeight & 0xF) < (n->cell.shapeAndHeight & 0xF))
                best = n;
        }
    }
    return (u8 *)best;
}

void TerrainModifiers_InitCell(struct TerrainCellOverride *, u32, u32, u32, u32, u32);

u32 TerrainModifiers_RegisterCell(struct TerrainCellOverride *n, u32 a, u32 b, u32 c, u32 d, u32 e)
{
    TerrainModifiers_InitCell(n, a, b, c, d, e);
    n->previous = 0;
    n->next = gTerrainMap->boxList;
    if (n->next)
        n->next->previous = n;
    gTerrainMap->boxList = n;
    return 0;
}

void TerrainModifiers_UnregisterCell(struct TerrainCellOverride *n)
{
    struct TerrainCellOverride *prev = n->previous;
    struct TerrainCellOverride *next = n->next;
    if (prev)
        prev->next = next;
    else
        gTerrainMap->boxList = next;
    if (next)
        next->previous = prev;
}

// confidence=low: tests higher adjacent cells across the selected subcell corner.

// confidence=low: compares base heights at positive subcell corners.

s32 Terrain_HasHigherCorner(struct ContactPosition *p, s32 threshold)
{
    s32 x = (p->x & 0xff00) + 128;
    s32 y = p->y & 0xff00;
    s32 z = (p->z & 0xff00) + 128;
    s32 index = cellIndexAt((struct Pos16 *)p);
    u8 *data = cellProps(index);
    s32 height;
    struct TerrainSignedProbe sample;
    u32 low;
    if (!data)
        return 0;
    height = *data & 15;
    low = 255;
    if ((u8)p->x < threshold) {
        low &= (u16)p->z;
        if ((s32)low < threshold)
            return 0;
        sample.x = x;
        sample.y = y;
        sample.z = z + 255;
        data = cellProps(cellIndexAt((struct Pos16 *)&sample));
    } else {
        low &= (u16)p->z;
        if ((s32)low >= threshold) {
            sample.x = x + 255;
            sample.y = y;
            sample.z = z;
            data = cellProps(cellIndexAt((struct Pos16 *)&sample));
            if (height < (*data & 15))
                return 1;
            sample.x = x;
            sample.y = y;
            sample.z = z + 255;
            data = cellProps(cellIndexAt((struct Pos16 *)&sample));
            if (height < (*data & 15))
                return 1;
            sample.x = x + 255;
            sample.y = y;
            sample.z = z + 255;
            data = cellProps(cellIndexAt((struct Pos16 *)&sample));
        } else {
            sample.x = x + 255;
            sample.y = y;
            sample.z = z;
            data = cellProps(cellIndexAt((struct Pos16 *)&sample));
        }
    }
    if (height >= (*data & 15))
        return 0;
    return 1;
}

// confidence=low: returns higher-cell flags used by SceneIndicator_CreateFromScript.

static inline u8 *CellData(s32 index)
{
    u8 *p = TerrainModifiers_FindCellOverride(index, 1);
    if (p)
        return (u8 *)&((struct TerrainCellOverride *)p)->cell;
    return (u8 *)&((struct TerrainGrid *)gTerrainMap->cells)->cells[index];
}

u32 Terrain_GetHigherNeighborMask(struct ContactPosition *p)
{
    struct TerrainProbe center, sample;
    u32 result = 0;
    u32 height;
    center.x = (p->x & ~255) | 128;
    center.y = p->y & ~255;
    center.z = (p->z & ~255) + 128;
    height = *CellData(Terrain_GetCellIndex((struct ContactPosition *)&center)) & 15;
    sample.x = center.x - 256;
    sample.y = center.y;
    sample.z = center.z + 256;
    if ((*CellData(Terrain_GetCellIndex((struct ContactPosition *)&sample)) & 15u) > height)
        result |= 1;
    sample.x = center.x + 0;
    sample.y = center.y;
    sample.z = center.z + 256;
    if ((*CellData(Terrain_GetCellIndex((struct ContactPosition *)&sample)) & 15u) > height)
        result |= 2;
    sample.x = center.x + 256;
    sample.y = center.y;
    sample.z = center.z + 256;
    if ((*CellData(Terrain_GetCellIndex((struct ContactPosition *)&sample)) & 15u) > height)
        result |= 4;
    sample.x = center.x + 256;
    sample.y = center.y;
    sample.z = center.z + 0;
    if ((*CellData(Terrain_GetCellIndex((struct ContactPosition *)&sample)) & 15u) > height)
        result |= 8;
    sample.x = center.x + 256;
    sample.y = center.y;
    sample.z = center.z - 256;
    if ((*CellData(Terrain_GetCellIndex((struct ContactPosition *)&sample)) & 15u) > height)
        result |= 16;
    return result;
}
