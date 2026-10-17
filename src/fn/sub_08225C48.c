#include "global.h"
/* HP resource header: offsets from its start (the nav meshes are optional). */
struct TerrainHeader {
    u32 f0;
    u32 cells;
    u32 boxes;
    u32 paths;
    u32 navMeshes;
};
void *ScriptProc_Start(u32, u16);
struct TerrainHeader *sub_08225C08(void);
void Terrain_SetGrid(u32);
void Terrain_SetBoxes(u32);
void Terrain_SetPaths(u32);
void Terrain_SetNavMeshes(u32);
/* Loads terrain resource id (tag AE6C) into the current terrain map: grid,
   named boxes, patrol paths and nav meshes; -1 if it is missing. */
s32 Terrain_LoadHP(u32 id)
{
    struct TerrainHeader *src, h;
    if (!ScriptProc_Start(0xAE6C, (u16)id))
        return -1;
    src = sub_08225C08();
    h = *src;
    h.cells += (u32)src;
    h.boxes += (u32)src;
    h.paths += (u32)src;
    if (h.navMeshes)
        h.navMeshes += (u32)src;
    Terrain_SetGrid(h.cells);
    Terrain_SetBoxes(h.boxes);
    Terrain_SetPaths(h.paths);
    Terrain_SetNavMeshes(h.navMeshes);
    return 0;
}
