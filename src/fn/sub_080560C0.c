#include "terrain_edge_marker.h"
void ParticleSprite_Initialize(struct TerrainMarkerSprite *, void *, u32);
void SpriteDrawable_SelectTileFrame(struct TerrainMarkerSprite *, void *, u32);
void Effect_SetOrigin(struct TerrainMarkerSprite *, s32, s32);
void TerrainEdgeMarker_Add(union TerrainMarkerPosition *position, s32 direction, s32 kind,
                           s32 preserveHeight)
{
    struct TerrainMarkerManager *manager = ResourceRegistry_GetResource(0xFD22);
    if (manager) {
        void *resource = AnimData_FindEntry(0x1C1A);
        struct TerrainMarkerSprite *marker = &manager->markers[manager->count];
        ParticleSprite_Initialize(marker, resource, 0);
        if (kind == 0)
            SpriteDrawable_SelectTileFrame(marker, resource, 0x33);
        else
            SpriteDrawable_SelectTileFrame(marker, resource, kind + 0x34);
        Effect_SetOrigin(marker, -8, -8);
        marker->position = *position;
        if (!preserveHeight) {
            s32 x = position->world.x >> 8;
            s32 z = position->world.z >> 8;
            u8 *cell;
            if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
                x = 0;
            else {
                s32 addend = x;
                u32 row = gTerrainMap->rows[z];
                x = row + addend;
            }
            {
                u8 *cells = gTerrainMap->cells;
                if (!cells)
                    cell = 0;
                else {
                    u32 offset = x * 4 + 12;
                    cell = cells + offset;
                }
            }
            marker->position.world.y = (*cell & 15) * 256 + 128;
        }
        marker->renderMode = 1;
        if (direction) {
            if (direction == 1)
                marker->flags |= 4;
            else if (direction == 2)
                marker->flags |= 12;
            else
                marker->flags |= 8;
        }
        manager->count++;
    }
}
