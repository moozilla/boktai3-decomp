#include "global.h"
#include "scene_actor_terrain.h"

s32 SceneActorTerrain_FindReachableCell(u16 *a, s32 dir, s32 n)
{
    s32 x = (s16)a[0] >> 8;
    s32 y = (s16)a[2] >> 8;
    s32 t, i, base, c;

    if (x < 0 || y < 0 || (u32)x >= gTerrainWidth || (u32)y >= gTerrainHeight)
        t = 0;
    else
        t = gTerrainMap->rows[y] + x;
    base = (*SceneActorTerrain_GetCell(t) & 15) << 8;
    c = t;
    switch (dir) {
        case 0:
            for (i = 0; i < n; i++) {
                if (base - ((*SceneActorTerrain_GetCell(c) & 15) << 8) < 0)
                    return c - 1;
                c++;
            }
            c -= 1;
            return c;
        case 1:
            for (i = 0; i < n; i++) {
                if (base - ((*SceneActorTerrain_GetCell(c) & 15) << 8) < 0)
                    return c - SceneActorTerrain_Width();
                c += SceneActorTerrain_Width();
            }
            c -= SceneActorTerrain_Width();
            return c;
        case 2:
            for (i = 0; i < n; i++) {
                if (base - ((*SceneActorTerrain_GetCell(c) & 15) << 8) < 0)
                    return c + 1;
                c--;
            }
            c += 1;
            return c;
        case 3:
            for (i = 0; i < n; i++) {
                if (base - ((*SceneActorTerrain_GetCell(c) & 15) << 8) < 0)
                    return c + SceneActorTerrain_Width();
                c -= SceneActorTerrain_Width();
            }
            c += SceneActorTerrain_Width();
            return c;
    }
    return t;
}
