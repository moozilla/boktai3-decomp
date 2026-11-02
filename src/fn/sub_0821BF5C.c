#include "global.h"
#include "engine.h"
#include "game_state.h"

struct TerrainPos {
    s16 x, y, z;
};
struct TerrainCell {
    u8 shapeAndHeight;
    u8 attribute;
    u16 flags;
};
struct TerrainCellList {
    u8 header[12];
    struct TerrainCell cells[1];
};
u8 *TerrainModifiers_FindCellOverride(s32, s32);
u16 sub_0821BE3C(struct TerrainPos *a, struct TerrainPos *b);

static inline s32 TerrainCell_GetIndex(struct TerrainPos *pos)
{
    s32 x = pos->x >> 8, z = pos->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

#define CELL_AT(cell, index)                                                                       \
    {                                                                                              \
        u8 *found = TerrainModifiers_FindCellOverride(index, 1);                                   \
        if (found)                                                                                 \
            cell = (struct TerrainCell *)(found + 4);                                              \
        else                                                                                       \
            cell = &((struct TerrainCellList *)gTerrainMap->cells)->cells[index];                  \
    }

static inline void TerrainCell_MoveToPreviousRow(s32 *cellIndex)
{
    *cellIndex -= gTerrainWidth;
}
/* Check whether the two terrain cells connect along their stair axes. */
u32 sub_0821BF5C(struct TerrainPos *from, struct TerrainPos *to, u8 kind)
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
    toIndex = TerrainCell_GetIndex(to);
    {
        u8 *found = TerrainModifiers_FindCellOverride(toIndex, 1);
        if (found)
            toCell = (struct TerrainCell *)(found + 4);
        else
            toCell = &((struct TerrainCellList *)gTerrainMap->cells)->cells[toIndex];
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
    if (sub_0821BE3C(from, to) > limit)
        goto blocked;
    fromIndex = TerrainCell_GetIndex(from);
    {
        u8 *found = TerrainModifiers_FindCellOverride(fromIndex, 1);
        if (found)
            fromCell = (struct TerrainCell *)(found + 4);
        else
            fromCell = &((struct TerrainCellList *)gTerrainMap->cells)->cells[fromIndex];
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
