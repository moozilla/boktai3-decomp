#include "global.h"
#include "engine.h"
#include "game_state.h"

#ifdef NONMATCHING
// fuzzy: 86.7%
struct TerrainPos {
    s16 x, y, z;
};
u8 *TerrainModifiers_FindCellOverride(s32, s32);
u16 sub_0821BE3C(struct TerrainPos *a, struct TerrainPos *b);

static inline s32 cellIndex(struct TerrainPos *pos)
{
    s32 x = pos->x >> 8, z = pos->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}

#define CELL_AT(cell, index)                                                                       \
    {                                                                                              \
        u8 *found = TerrainModifiers_FindCellOverride(index, 1);                                   \
        if (found)                                                                                 \
            cell = found + 4;                                                                      \
        else                                                                                       \
            cell = gUnk_030052F4->cells + 12 + (index) * 4;                                        \
    }

u32 sub_0821BF5C(struct TerrainPos *from, struct TerrainPos *to, u8 kind)
{
    u16 toIndex, fromIndex;
    u8 *toCell, *fromCell, *cell, *other, *found;
    u16 toFlags;
    u32 mask;
    s32 limit, diff, n;
    u32 fromKind, toKind;
    u32 height;

    toIndex = cellIndex(to);
    CELL_AT(toCell, toIndex);
    toFlags = *(u16 *)(toCell + 2);
    if (kind == 4) {
        mask = 2;
        limit = 0x80;
    } else {
        mask = 1;
        limit = 100;
    }
    if (toFlags & mask)
        return 0;
    if (sub_0821BE3C(from, to) > limit)
        return 0;
    fromIndex = cellIndex(from);
    CELL_AT(fromCell, fromIndex);
    fromKind = *fromCell >> 4;
    toKind = *toCell >> 4;
    diff = toIndex - fromIndex;
    if (fromKind == 1) {
        if (diff != 1 && diff != -1)
            return 1;
        if ((gUnk_02000710->flags868 & 0x10) && (toFlags & 0x80))
            return 0;
        if (toKind != 0)
            return 1;
        if ((to->z & 0xFF) <= 0x7F) {
            n = toIndex - gUnk_030052F8;
            found = TerrainModifiers_FindCellOverride(n, 1);
        } else {
            n = toIndex + gUnk_030052F8;
            found = TerrainModifiers_FindCellOverride(n, 1);
        }
        if (found)
            cell = found + 4;
        else
            cell = gUnk_030052F4->cells + 12 + n * 4;
        if (*cell >> 4)
            return 0;
        if ((*cell & 15) != (*toCell & 15))
            return 0;
        if (*(u16 *)(cell + 2) & mask)
            return 0;
        return 1;
    } else if (fromKind == 2) {
        if (diff != gUnk_030052F8 && diff != -gUnk_030052F8)
            return 1;
        if ((gUnk_02000710->flags868 & 0x10) && (toFlags & 0x80))
            return 0;
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
            cell = gUnk_030052F4->cells + 12 + n * 4;
        if (*cell >> 4)
            return 0;
        if ((*cell & 15) != (*toCell & 15))
            return 0;
        if (*(u16 *)(cell + 2) & mask)
            return 0;
        return 1;
    } else {
        if (diff == gUnk_030052F8 || diff == -gUnk_030052F8) {
            if (toKind != 2)
                return 1;
            if ((to->x & 0xFF) <= 0x7F) {
                n = fromIndex - 1;
                found = TerrainModifiers_FindCellOverride(n, 1);
            } else {
                n = fromIndex + 1;
                found = TerrainModifiers_FindCellOverride(n, 1);
            }
            if (found)
                cell = found + 4;
            else
                cell = gUnk_030052F4->cells + 12 + n * 4;
        } else if (diff == 1 || diff == -1) {
            if (toKind != 1)
                return 1;
            if ((to->z & 0xFF) <= 0x7F) {
                n = fromIndex - gUnk_030052F8;
                found = TerrainModifiers_FindCellOverride(n, 1);
            } else {
                n = fromIndex + gUnk_030052F8;
                found = TerrainModifiers_FindCellOverride(n, 1);
            }
            if (found)
                cell = found + 4;
            else
                cell = gUnk_030052F4->cells + 12 + n * 4;
        } else
            return 1;
        if (*cell >> 4)
            return 0;
        if ((*cell & 15) != (*fromCell & 15))
            return 0;
        if (*(u16 *)(cell + 2) & mask)
            return 0;
        return 1;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0821BF5C);
#endif
