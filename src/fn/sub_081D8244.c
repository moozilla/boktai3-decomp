#include "global.h"
#include "scene_actor_terrain.h"

static inline s32 CellIndex(u16 *a)
{
    s32 x = ((s16)a[0]) >> 8;
    s32 z = ((s16)a[2]) >> 8;
    if ((((x < 0) || (z < 0)) || (((u32)x) >= gTerrainWidth)) || (((u32)z) >= gTerrainHeight))
        return 0;
    return gTerrainMap->rows[z] + x;
}
s32 Task4E01_ResolveTerrainDirection(u16 *, s32);
s32 sub_08249350(s32, s32);
static inline s16 SignedCoordinate(u16 arg0)
{
    return (s16)arg0;
}

// confidence=low: turns a projectile around the current terrain cell using its travel angle.
s32 Task4E01_SelectTerrainTurn(u16 *a, u16 *b, s32 original)
{
    u16 *centerCoords;
    s16 direction = original;
    s32 next = 0;
    s32 cell;
    s32 other;
    s32 width;
    s32 far;
    struct ContactPosition center;
    u32 angle;
    s32 turnAngle;
    s32 centerX;
    TerrainModifiers_FindCellOverride(CellIndex(a), 1);
    if (direction & 1) {
        direction += 4;
        if (direction > 7) {
            direction -= 8;
        }
        return direction;
    }
    cell = CellIndex(a);
    other = CellIndex(b);
    width = gTerrainWidth;
    if (((((cell + 1) == other) || ((cell - 1) == other)) || ((cell + width) == other)) ||
        ((cell - width) == other)) {
        far = 0;
    } else {
        far = 1;
    }
    centerX = (sub_08249350(cell, width) << 8) + 128;
    center.x = centerX;
    center.y = 0;
    center.z = ((cell / width) << 8) + 128;
    centerCoords = (u16 *)&center;
    turnAngle = Math_Atan2(SignedCoordinate(a[0]) - SignedCoordinate(centerCoords[0]),
                           SignedCoordinate(a[2]) - SignedCoordinate(centerCoords[2]));
    angle = turnAngle;
    if (direction == 0) {
        if ((angle - 33) <= 126) {
            direction -= 2;
            next = cell + width;
        } else if ((angle - 32) > 128) {
            direction = 2;
            next = cell + 1;
        } else {
            direction = Task4E01_ResolveTerrainDirection(a, 0);
        }
    } else if (direction == 2) {
        if ((angle - 97) <= 126) {
            direction = 0;
            next = cell - 1;
        } else if ((angle - 96) > 128) {
            direction = 4;
            next = cell + width;
        } else {
            direction = Task4E01_ResolveTerrainDirection(a, 2);
        }
    } else if (direction == 4) {
        if ((angle - 32) > 128) {
            direction = 2;
            next = cell - width;
        } else if ((angle - 33) <= 126) {
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
        direction = Task4E01_ResolveTerrainDirection(a, direction);
    }
    if (direction > 7) {
        direction -= 8;
    } else if (0 > direction) {
        direction += 8;
    }
    if (((direction - original) != 4) && ((original - direction) != 4)) {
        if (far) {
            u32 height = SignedCoordinate(a[1]);
            if (height <= (((*SceneActorTerrain_GetCell(next)) & 15) << 8)) {
                direction += 4;
            }
        }
        direction = Mod(direction, 8);
    }
    return direction;
}
