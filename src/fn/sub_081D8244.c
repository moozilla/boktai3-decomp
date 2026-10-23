#include "global.h"
#ifdef NONMATCHING
// fuzzy: 93.3%
// fix.py best version (reprinted C)
// Remaining differences: center-vector stack layout, branch sharing and registers.
#include "scene_actor_terrain.h"

static inline s32 CellIndex(u16 *a)
{
    s32 x = ((s16)a[0]) >> 8;
    s32 z = ((s16)a[2]) >> 8;
    if ((((x < 0) || (z < 0)) || (((u32)x) >= gUnk_030052F8)) || (((u32)z) >= gUnk_030052FC))
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
s32 Task4E01_ResolveTerrainDirection(u16 *, s32);
s32 sub_08249350(s32, s32);
static inline s16 inline_fn(u16 arg0)
{
    return (s16)arg0;
}

// confidence=low: turns a projectile around the current terrain cell using its travel angle.
s32 Task4E01_SelectTerrainTurn(u16 *a, u16 *b, s32 original)
{
    u16 *new_var;
    u16 direction = original;
    s32 next = 0;
    s32 cell;
    s32 other;
    s32 width;
    s32 far;
    u16 center[3];
    s8 k2;
    u32 angle;
    typeof(Math_Atan2(inline_fn(a[0]) - inline_fn(new_var[0]),
                      inline_fn(a[2]) - inline_fn(new_var[2]))) t1;
    typeof((sub_08249350(cell, width) << 8) + 128) t2;
    TerrainModifiers_FindCellOverride(CellIndex(a), 1);
    if (inline_fn(direction) & (k2 = 1)) {
        direction += 4;
        if (inline_fn(direction) > 7) {
            direction -= 8;
        }
        return inline_fn(direction);
    }
    cell = CellIndex(a);
    other = CellIndex(b);
    width = gUnk_030052F8;
    if (((((cell + 1) == other) || ((cell - 1) == other)) || ((cell + width) == other)) ||
        ((cell - width) == other)) {
        far = 0;
    } else {
        far = 1;
    }
    t2 = (sub_08249350(cell, width) << 8) + 128;
    center[0] = t2;
    center[2] = ((cell / width) << 8) + 128;
    new_var = center;
    t1 = Math_Atan2(inline_fn(a[0]) - inline_fn(new_var[0]),
                    inline_fn(a[2]) - inline_fn(new_var[2]));
    angle = t1;
    if (inline_fn(direction) == 0) {
        if ((angle - 33) <= 126) {
            direction -= 2;
            next = cell + width;
        } else if ((angle - 32) > 128) {
            direction = 2;
            next = cell + 1;
        } else {
            direction = Task4E01_ResolveTerrainDirection(a, 0);
        }
    } else if (inline_fn(direction) == 2) {
        if ((angle - 97) <= 126) {
            direction = 0;
            next = cell - 1;
        } else if ((angle - 96) > 128) {
            direction = 4;
            next = cell + width;
        } else {
            direction = Task4E01_ResolveTerrainDirection(a, 2);
        }
    } else if (inline_fn(direction) == 4) {
        if ((angle - 32) > 128) {
            direction = 2;
            next = cell - width;
        } else if ((angle + (-33)) <= 126) {
            direction = 6;
            next = cell - 1;
        } else {
            direction = Task4E01_ResolveTerrainDirection(a, 4);
        }
    } else if ((angle - 96) > 128) {
        direction -= 2;
        next = cell + 1;
    } else if ((angle - 97) <= 126) {
        direction += 2;
        next = cell - width;
    } else {
        direction = Task4E01_ResolveTerrainDirection(a, inline_fn(direction));
    }
    if (((s16)direction) > 7) {
        direction -= 8;
    } else if (0 > ((s16)direction)) {
        direction += 8;
    }
    if (((((s16)direction) - original) != 4) && ((original - ((s16)direction)) != 4)) {
        if (far) {
            u32 height = inline_fn(a[1]);
            if (height <= (((*SceneActorTerrain_GetCell(next)) & 15) << 8)) {
                direction += 4;
            }
        }
        direction = Mod(inline_fn(direction), 8);
    }
    return (s16)direction;
}
#else
INCLUDE_ASM("asm/nonmatching", Task4E01_SelectTerrainTurn);
#endif
