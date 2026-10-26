#include "global.h"
#include "engine.h"
#include "game_state.h"

#ifdef NONMATCHING
// fuzzy: 79.5%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
struct TerrainPos {
    s16 x, y, z;
};
u8 *TerrainModifiers_FindCellOverride(s32, s32);
u16 sub_0821BE3C(struct TerrainPos *a, struct TerrainPos *b);

static inline s32 cellIndex(struct TerrainPos *pos)
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
            cell = found + 4;                                                                      \
        else                                                                                       \
            cell = gTerrainMap->cells + 12 + (index) * 4;                                          \
    }

u32 sub_0821BF5C(struct TerrainPos *from, struct TerrainPos *to, u8 kind)
{
    u16 toIndex;
    u16 fromIndex;
    u8 *toCell;
    u8 *fromCell;
    u8 *cell;
    u8 *other;
    u8 *found;
    u16 toFlags;
    u32 mask;
    s32 limit;
    s32 diff;
    s32 n;
    u32 fromKind;
    u32 toKind;
    u32 height;
    toIndex = cellIndex(to);
    {
        u8 *found = TerrainModifiers_FindCellOverride(toIndex, 1);
        if (found)
            toCell = found + 4;
        else
            toCell = (gTerrainMap->cells + 12) + (toIndex * 4);
    };
    toFlags = *((u16 *)(toCell + 2));
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
    fromIndex = cellIndex(from);
    {
        u8 *found = TerrainModifiers_FindCellOverride(fromIndex, 1);
        if (found)
            fromCell = found + 4;
        else
            fromCell = (gTerrainMap->cells + 12) + (fromIndex * 4);
    };
    fromKind = (*fromCell) >> 4;
    toKind = (*toCell) >> 4;
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
            found = TerrainModifiers_FindCellOverride(n, 1);
        } else {
            n = toIndex + gTerrainWidth;
            found = TerrainModifiers_FindCellOverride(n, 1);
        }
        if (found)
            cell = found + 4;
        else
            cell = (gTerrainMap->cells + 12) + (n * 4);
        if ((*cell) >> 4)
            goto ret1;
        if (((*cell) & 15) != ((*toCell) & 15))
            goto ret1;
        if ((*((u16 *)(cell + 2))) & mask)
            goto ret1;
        return 1;
    } else if (fromKind == 2) {
        if ((diff != gTerrainWidth) && (diff != (-gTerrainWidth)))
            return 1;
        if ((gGameState->flags868 & 0x10) && (toFlags & 0x80))
            goto ret1;
        if (toKind != 0)
            return 1;
        if ((to->x & 0xFF) <= 0x7F) {
            n = toIndex - 1;
            found = TerrainModifiers_FindCellOverride(n, 1);
        } else {
            n = toIndex + 1;
            found = TerrainModifiers_FindCellOverride(n, 1);
        }
        if (found)
            cell = found + 4;
        else
            cell = (gTerrainMap->cells + 12) + (n * 4);
        if ((*cell) >> 4)
            goto ret1;
        if (((*cell) & 15) != ((*toCell) & 15))
            goto ret1;
        if ((*((u16 *)(cell + 2))) & mask)
            goto ret1;
        return 1;
    } else {
        if ((diff == gTerrainWidth) || (diff == (-gTerrainWidth))) {
            if (toKind != 2)
                return 1;
            if ((to->x & 0xFF) <= 0x7F) {
                n = fromIndex - 1;
                found = TerrainModifiers_FindCellOverride(n, 1);
            } else {
                n = fromIndex + 1;
                found = TerrainModifiers_FindCellOverride(n, 1);
            }
            if (!found)
                cell = (gTerrainMap->cells + 12) + (n * 4);
            else
                cell = found + 4;
        } else if ((diff == 1) || (diff == (-1))) {
            if (toKind != 1)
                return 1;
            if ((to->z & 0xFF) <= 0x7F) {
                n = fromIndex - gTerrainWidth;
                found = TerrainModifiers_FindCellOverride(n, 1);
            } else {
                n = fromIndex + gTerrainWidth;
                found = TerrainModifiers_FindCellOverride(n, 1);
            }
            if (found)
                cell = found + 4;
            else
                cell = (gTerrainMap->cells + 12) + (n * 4);
        } else
            return 1;
        if ((*cell) >> 4)
            goto ret1;
        if (((*cell) & 15) != ((*fromCell) & 15))
            goto ret1;
        if ((*((u16 *)(cell + 2))) & mask)
            goto ret1;
        return 1;
    }
ret1:
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0821BF5C);
#endif
