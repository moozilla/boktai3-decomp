// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// fuzzy: 84.8%
// Remaining differences: cell-index register reuse and height-test ordering.
#include "scene_actor_terrain.h"

static inline s32 CellIndex(u16 *a)
{
    s32 x = (s16)a[0] >> 8;
    s32 z = (s16)a[2] >> 8;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
static inline u32 AboveCell(u16 *a, s32 cell)
{
    u32 height = *SceneActorTerrain_GetCell(cell) & 15;
    u32 above = 0;
    if ((s16)a[1] > (height << 8)) above = 1;
    return above;
}
// confidence=low: selects a cardinal direction from adjacent terrain heights for diagonal travel.
#define Projectile_ResolveTerrainDirection sub_081D7F8C
s32 Projectile_ResolveTerrainDirection(u16 *input, s32 direction)
{
    register u16 *a asm("r5") = input;
    s32 cell, width;
    register u32 first asm("r6");
    register u32 second asm("r2");
    TerrainModifiers_FindCellOverride(CellIndex(a), 1);
    cell = CellIndex(a);
    width = gUnk_030052F8;
    if (direction == 0) {
        first = AboveCell(a, cell + 1);
        second = AboveCell(a, cell + width);
        if (first != second) {
            direction = 2;
            if (second) direction = 6;
        } else direction = 4;
    } else if (direction == 2) {
        first = AboveCell(a, cell - 1);
        second = AboveCell(a, cell + width);
        if (first == second) direction = 6;
        else direction = second ? 4 : 0;
    } else if (direction == 4) {
        first = AboveCell(a, cell - 1);
        second = AboveCell(a, cell - width);
        if (first != second) {
            direction = 6;
            if (second) direction = 2;
        } else direction = 0;
    } else if (direction == 6) {
        first = AboveCell(a, cell + 1);
        second = AboveCell(a, cell - width);
        if (first != second) {
            direction = 4;
            if (second) direction = 0;
        } else direction = 2;
    }
    return direction;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081D7F8C);
#endif
