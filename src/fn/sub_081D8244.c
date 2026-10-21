#include "global.h"
#ifdef NONMATCHING
// fuzzy: 85.5%
// Remaining differences: center-vector stack layout, branch sharing and registers.
#include "scene_actor_terrain.h"

static inline s32 CellIndex(u16 *a)
{
    s32 x = (s16)a[0] >> 8;
    s32 z = (s16)a[2] >> 8;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
s32 Task4E01_ResolveTerrainDirection(u16 *, s32);
s32 sub_08249350(s32, s32);
// confidence=low: turns a projectile around the current terrain cell using its travel angle.
s32 Task4E01_SelectTerrainTurn(u16 *a, u16 *b, s32 original)
{
    u16 direction = original;
    s32 next = 0;
    s32 cell, other, width, far;
    u16 center[3];
    u32 angle;
    TerrainModifiers_FindCellOverride(CellIndex(a), 1);
    if ((s16)direction & 1) {
        direction += 4;
        if ((s16)direction > 7)
            direction -= 8;
        return (s16)direction;
    }
    cell = CellIndex(a);
    other = CellIndex(b);
    width = gUnk_030052F8;
    if (cell + 1 == other || cell - 1 == other || cell + width == other || cell - width == other)
        far = 0;
    else
        far = 1;
    center[0] = (sub_08249350(cell, width) << 8) + 128;
    center[2] = (cell / width << 8) + 128;
    angle = Math_Atan2((s16)a[0] - (s16)center[0], (s16)a[2] - (s16)center[2]);
    if ((s16)direction == 0) {
        if (angle - 33 <= 126) {
            direction -= 2;
            next = cell + width;
        } else if (angle - 32 > 128) {
            direction = 2;
            next = cell + 1;
        } else
            direction = Task4E01_ResolveTerrainDirection(a, 0);
    } else if ((s16)direction == 2) {
        if (angle - 97 <= 126) {
            direction = 0;
            next = cell - 1;
        } else if (angle - 96 > 128) {
            direction = 4;
            next = cell + width;
        } else
            direction = Task4E01_ResolveTerrainDirection(a, 2);
    } else if ((s16)direction == 4) {
        if (angle - 32 > 128) {
            direction = 2;
            next = cell - width;
        } else if (angle - 33 <= 126) {
            direction = 6;
            next = cell - 1;
        } else
            direction = Task4E01_ResolveTerrainDirection(a, 4);
    } else {
        if (angle - 96 > 128) {
            direction -= 2;
            next = cell + 1;
        } else if (angle - 97 <= 126) {
            direction += 2;
            next = cell - width;
        } else
            direction = Task4E01_ResolveTerrainDirection(a, (s16)direction);
    }
    if ((s16)direction > 7)
        direction -= 8;
    else if ((s16)direction < 0)
        direction += 8;
    if ((s16)direction - original != 4 && original - (s16)direction != 4) {
        if (far) {
            u32 height = (s16)a[1];
            if (height <= ((*SceneActorTerrain_GetCell(next) & 15) << 8))
                direction += 4;
        }
        direction = Mod((s16)direction, 8);
    }
    return (s16)direction;
}

#else
INCLUDE_ASM("asm/nonmatching", Task4E01_SelectTerrainTurn);
#endif
