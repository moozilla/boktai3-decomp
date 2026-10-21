// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// fuzzy: 96.8%
// Remaining differences: cell-index register reuse and height-test ordering.
#include "scene_actor_terrain.h"

static inline s32 CellIndex(u16 *a)
{
    s32 x = ((s16)a[0]) >> 8;
    s32 z = ((s16)a[2]) >> 8;
    if ((((x < 0) || (z < 0)) || (((u32)x) >= gUnk_030052F8)) || (((u32)z) >= gUnk_030052FC))
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
static inline u32 AboveCell(u16 *a, s32 cell)
{
    u32 height = *SceneActorTerrain_GetCell(cell) & 15;
    u32 above = 0;
    if ((s16)a[1] > (height << 8))
        above = 1;
    return above;
}
// confidence=low: selects a cardinal direction from adjacent terrain heights for diagonal travel.
s32 Task4E01_ResolveTerrainDirection(u16 *input, s32 direction)
{
    s32 cell;
    s32 width;
    register u32 first asm("r6");
    register u32 second asm("r2");
    TerrainModifiers_FindCellOverride(CellIndex(input), 1);
    width = gUnk_030052F8;
    cell = CellIndex(input);
    if (direction == 0) {
        first = AboveCell(input, cell + 1);
        second = AboveCell(input, cell + width);
        if (first != second) {
            direction = 2;
            if (second) {
                direction = 6;
            }
        } else {
            direction = 4;
        }
    } else if (direction == 2) {
        first = AboveCell(input, cell - 1);
        second = AboveCell(input, cell + width);
        if (first == second) {
            direction = 6;
        } else {
            direction = (second) ? (4) : (0);
        }
    } else if (direction == 4) {
        first = AboveCell(input, cell - 1);
        second = AboveCell(input, cell - width);
        if (first != second) {
            direction = 6;
            if (second) {
                direction = 2;
            }
        } else {
            direction = 0;
        }
    } else if (direction == 6) {
        first = AboveCell(input, cell + 1);
        second = AboveCell(input, cell - width);
        if (first != second) {
            direction = 4;
            if (second) {
                direction = 0;
            }
        } else {
            direction = 2;
        }
    }
    return direction;
}

#else
INCLUDE_ASM("asm/nonmatching", Task4E01_ResolveTerrainDirection);
#endif
