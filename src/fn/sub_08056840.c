#include "terrain_edge_marker.h"
struct TerrainMarkerRegion *Terrain_FindBoxesById(u16, u16 *);
u8 *TerrainModifiers_FindCellOverride(u32, u32);
void TerrainEdgeMarker_AddRegionFromScript(void)
{
    u16 count;
    union TerrainMarkerPosition center;
    struct TerrainMarkerRegion *region;
    s32 tag;
    if (Script_SeekToKeyword('n'))
        tag = Script_GetValue();
    else
        tag = 0;
    region = Terrain_FindBoxesById((u16)tag, &count);
    if (region) {
        s32 x;
        s32 z;
        s32 index;
        union TerrainMarkerPosition *sample;
        union TerrainMarkerPosition *world;
        u8 *cell;
        s32 type;
        s32 height;
        s32 direction;
        {
            u32 mask;
            u32 value = (u16)((region->minX + region->maxX) >> 1);
            mask = 0xFFFF0000;
            center.words[0] = (center.words[0] & mask) | value;
            {
                u32 value = (u16)((region->minZ + region->maxZ) >> 1);
                union TerrainMarkerPosition *builder = &center;
                {
                    u32 word = builder->words[1];
                    word &= mask;
                    word |= value;
                    builder->words[1] = word;
                }
                sample = builder;
            }
        }

        {
            u32 value = sample->packed.x;
            value <<= 16;
            x = (s32)value >> 24;
        }
        z = sample->world.z >> 8;
        world = sample;
        if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
            index = 0;
        else
            index = gTerrainMap->rows[z] + x;
        cell = TerrainModifiers_FindCellOverride(index, 1);
        if (cell)
            cell += 4;
        else {
            struct EngineMap *map = gTerrainMap;
            u32 offset = index * 4 + 12;
            cell = map->cells + offset;
        }
        type = *cell >> 4;
        height = (*cell & 15) << 8;
        switch (type) {
            case 1:
                height -= *(u8 *)&sample->world.z;
                break;
            case 2:
                height -= *(u8 *)&sample->world.x;
                break;
        }
        world->world.y = height;
        direction = TerrainEdgeMarker_FindTerrainDirection(region);
        if (direction >= 0) {
            s32 kind;
            if (Script_SeekToKeyword('t'))
                kind = Script_GetValue();
            else
                kind = 0;
            TerrainEdgeMarker_Add(world, direction, kind, 0);
        }
    }
}
