// MATCHING_CONCESSION: register-pinning
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
    register s32 diff asm("r2");
    s32 n;
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
        goto ret1;
    if (sub_0821BE3C(from, to) > limit)
        goto ret1;
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
    diff = toIndex - fromIndex;
    if (fromKind == 1) {
        if ((diff != 1) && (diff != (-1)))
            return 1;
        if ((gGameState->flags868 & 0x10) && (toFlags & 0x80))
            goto ret1;
        if (toKind != 0)
            return 1;
        if ((to->z & 0xFF) <= 0x7F) {
            n = toIndex - gTerrainWidth;
            CELL_AT(cell, n);
        } else {
            n = toIndex + gTerrainWidth;
            CELL_AT(cell, n);
        }
        {
            u32 cellHeight, referenceHeight;
            if (cell->shapeAndHeight >> 4)
                goto ret1;
            cellHeight = cell->shapeAndHeight & 15;
            referenceHeight = toCell->shapeAndHeight & 15;
            if (cellHeight != referenceHeight)
                goto ret1;
            if (!(cell->flags & mask))
                goto success;
            goto ret1;
        }
    } else if (fromKind == 2) {
        if ((diff != gTerrainWidth) && (diff != (-gTerrainWidth)))
            return 1;
        if ((gGameState->flags868 & 0x10) && (toFlags & 0x80))
            goto ret1;
        if (toKind != 0)
            return 1;
        if ((to->x & 0xFF) <= 0x7F) {
            n = toIndex - 1;
            CELL_AT(cell, n);
        } else {
            n = toIndex + 1;
            CELL_AT(cell, n);
        }
        {
            u32 cellHeight, referenceHeight;
            if (cell->shapeAndHeight >> 4)
                goto ret1;
            cellHeight = cell->shapeAndHeight & 15;
            referenceHeight = toCell->shapeAndHeight & 15;
            if (cellHeight != referenceHeight)
                goto ret1;
            if (!(cell->flags & mask))
                goto success;
            goto ret1;
        }
    } else {
        n = fromIndex;
        if ((diff == gTerrainWidth) || (diff == (-gTerrainWidth))) {
            if (toKind != 2)
                return 1;
            if ((to->x & 0xFF) <= 0x7F) {
                n--;
                goto lookup;
            } else {
                n++;
                goto lookup;
            }
        } else if ((diff == 1) || (diff == (-1))) {
            if (toKind != 1)
                return 1;
            if ((to->z & 0xFF) <= 0x7F) {
                n -= gTerrainWidth;
            lookup:
                CELL_AT(cell, n);
            } else {
                n += gTerrainWidth;
                CELL_AT(cell, n);
            }
        } else
            return 1;
        {
            u32 cellHeight, referenceHeight;
            if (cell->shapeAndHeight >> 4)
                goto ret1;
            cellHeight = cell->shapeAndHeight & 15;
            referenceHeight = fromCell->shapeAndHeight & 15;
            if (cellHeight != referenceHeight)
                goto ret1;
            if (!(cell->flags & mask))
                goto success;
            goto ret1;
        }
    }
ret1:
    return 0;
success:
    return 1;
}
