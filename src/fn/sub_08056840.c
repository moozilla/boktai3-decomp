// MATCHING_CONCESSION: register-pinning
#include "terrain_edge_marker.h"
#define TerrainEdgeMarker_AddRegionFromScript sub_08056840
struct TerrainMarkerRegion *sub_0821E104(u16, u16 *);
u8 *sub_0821D64C(u32, u32);
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
    region = sub_0821E104((u16)tag, &count);
    if (region) {
        s32 x;
        s32 z, index;
        register union TerrainMarkerPosition *sample asm("r6");
        union TerrainMarkerPosition *world;
        u8 *cell;
        s32 type, height, direction;
        {
            u32 mask;
            u32 value = (u16)((region->minX + region->maxX) >> 1);
            mask = 0xFFFF0000;
            center.words[0] = (center.words[0] & mask) | value;
            {
                u32 value = (u16)((region->minZ + region->maxZ) >> 1);
                register union TerrainMarkerPosition *builder asm("r2") = &center;
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
        if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
            index = 0;
        else
            index = gUnk_030052F4->rows[z] + x;
        cell = sub_0821D64C(index, 1);
        if (cell)
            cell += 4;
        else {
            struct EngineMap *map = gUnk_030052F4;
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
        direction = sub_08056298(region);
        if (direction >= 0) {
            s32 kind;
            if (Script_SeekToKeyword('t'))
                kind = Script_GetValue();
            else
                kind = 0;
            sub_080560C0(world, direction, kind, 0);
        }
    }
}
