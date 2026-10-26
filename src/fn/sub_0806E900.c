// MATCHING_CONCESSION: register-pinning
#include "scene_grid.h"
u8 *TerrainModifiers_FindCellOverride(s32, s32);
static inline u8 *Properties(s32 index)
{
    u8 *p = TerrainModifiers_FindCellOverride(index, 1);
    if (p)
        return p + 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = index * 4 + 12;
        return map->cells + offset;
    }
}
static inline u32 Blocked(u8 *properties)
{
    u32 flags = *(u16 *)(properties + 2);
    flags &= 0x80;
    return flags;
}

/* Directional height scan; stopping relies on terrain flag 0x80. */
s32 SceneGrid_ScanTerrainDirection(s32 start, s32 angle)
{
    s32 savedStart = start;
    s32 cell;
    register s32 direction asm("r4");
    register s32 height asm("r9");
    s32 best;
    direction = (s32)((u32)angle + 32) >> 6;
    direction &= 3;
    {
        u32 raw;
        register u32 mask asm("r1");
        u8 *properties = TerrainModifiers_FindCellOverride(savedStart, 1);
        if (properties)
            properties += 4;
        else {
            struct EngineMap *map = gTerrainMap;
            u32 offset = savedStart * 4 + 12;
            properties = map->cells + offset;
        }
        raw = *properties;
        mask = 15;
        asm("" : "+r"(mask));
        height = mask;
        asm("" : "+r"(height));
        {
            register u32 low asm("r3") = height;
            asm("" : "+r"(low));
            low &= raw;
            height = low;
        }
    }
    cell = savedStart;
    best = 0;
    switch (direction) {
        case 0: {
            struct EngineMap **location = &gTerrainMap;
            u32 offset;
            {
                u32 initial = savedStart * 4;
                offset = initial + 16;
            }
            for (;;) {
                s32 next = cell + 1;
                register s32 adjacentHeight asm("r5");
                u8 *properties = TerrainModifiers_FindCellOverride(next, 1);
                if (properties)
                    properties += 4;
                else {
                    struct EngineMap **low = location;
                    properties = (*low)->cells + offset;
                }
                {
                    u32 raw = *properties;
                    adjacentHeight = 15;
                    adjacentHeight &= raw;
                }
                properties = TerrainModifiers_FindCellOverride(next, 1);
                if (properties)
                    properties += 4;
                else {
                    struct EngineMap **low = location;
                    properties = (*low)->cells + offset;
                }
                if (Blocked(properties)) {
                    if (best)
                        return best;
                    return cell;
                }
                if (adjacentHeight != height) {
                    if (best)
                        return best;
                    best = cell;
                } else {
                    best = next;
                }
                offset += 4;
                cell = cell + 1;
            }
        }
        case 1:
            for (;;) {
                s32 next = cell + SceneGrid_Width();
                s32 adjacentHeight = *Properties(next) & 15;
                if (Blocked(Properties(next))) {
                    if (best)
                        return best;
                    return cell;
                }
                if (adjacentHeight != height) {
                    if (best)
                        return best;
                    best = cell;
                } else {
                    best = next;
                }
                cell = cell + SceneGrid_Width();
            }
        case 2: {
            struct EngineMap **location = &gTerrainMap;
            u32 offset;
            {
                u32 initial = cell * 4;
                offset = initial + 8;
            }
            for (;;) {
                s32 next = cell - 1;
                register s32 adjacentHeight asm("r5");
                u8 *properties = TerrainModifiers_FindCellOverride(next, 1);
                if (properties)
                    properties += 4;
                else {
                    struct EngineMap **low = location;
                    properties = (*low)->cells + offset;
                }
                {
                    u32 raw = *properties;
                    adjacentHeight = 15;
                    adjacentHeight &= raw;
                }
                properties = TerrainModifiers_FindCellOverride(next, 1);
                if (properties)
                    properties += 4;
                else {
                    struct EngineMap **low = location;
                    properties = (*low)->cells + offset;
                }
                if (Blocked(properties)) {
                    if (best)
                        return best;
                    return cell;
                }
                if (adjacentHeight != height) {
                    if (best)
                        return best;
                    best = cell;
                } else {
                    best = next;
                }
                offset -= 4;
                cell = cell - 1;
            }
        }
        case 3:
            for (;;) {
                s32 next = cell - SceneGrid_Width();
                s32 adjacentHeight = *Properties(next) & 15;
                if (Blocked(Properties(next))) {
                    if (best)
                        return best;
                    return cell;
                }
                if (adjacentHeight != height) {
                    if (best)
                        return best;
                    best = cell;
                } else {
                    best = next;
                }
                cell = cell - SceneGrid_Width();
            }
    }
    return savedStart;
}
