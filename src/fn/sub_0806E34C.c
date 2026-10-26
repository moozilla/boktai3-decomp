// MATCHING_CONCESSION: register-pinning
#include "scene_grid.h"
#include "contact_actor.h"
/* SceneGrid_TryRandomNearbyPosition: Samples one nearby grid position and writes its cell center only if the board entry matches the source terrain height */
#define SceneGrid_TryRandomNearbyPosition sub_0806E34C
s32 Div(s32, s32);
s32 Mod(s32, s32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
extern const s8 gUnk_08605BC8[];
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
#define SceneGrid_TryRandomNearbyPosition sub_0806E34C

s32 SceneGrid_TryRandomNearbyPosition(struct ContactPosition *output, void *unused,
                                      const struct ContactPosition *origin, s32 radius)
{
    register s32 spread asm("r5") = radius;
    s32 x = origin->x >> 8;
    register s32 z asm("r1");
    register s32 index asm("r6");
    s32 column;
    s32 row;
    s32 height;
    s32 baseColumn;
    s32 baseRow;
    const u16 *samples;
    s32 chosenColumn;
    {
        register u32 rawZ asm("r0") = (u16)origin->z;
        z = (s16)rawZ >> 8;
    }
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        index = 0;
    else {
        register u32 rowBase asm("r0") = gTerrainMap->rows[z];
        index = rowBase + x;
    }
    column = Mod(index, SceneGrid_Width());
    row = Div(index, SceneGrid_Width());
    height = *Properties(index) & 15;
    baseColumn = column - spread;
    baseRow = row - spread;
    spread = spread * 2 + 1;
    {
        const u16 *base = gUnk_0203B400;
        samples = base;
    }
    gRandomIndex = (gRandomIndex + 1) & 1023;
    {
        s32 choice = Mod(samples[gRandomIndex], spread);
        s32 base = baseColumn;
        chosenColumn = choice + base;
    }
    gRandomIndex = (gRandomIndex + 1) & 1023;
    {
        s32 choice = Mod(samples[gRandomIndex], spread);
        s32 chosenRow;
        s32 base = baseRow;
        chosenRow = choice + base;
        {
            const s8 *table = gUnk_08605BC8;
            register s32 offset asm("r0");
            offset = chosenRow * 8 - 69;
            {
                const u8 *address;
                u32 raw;
                offset = chosenColumn + offset;
                address = (void *)(offset + (u32)table);
                raw = *address;
                raw = (s8)raw;
                {
                    s32 expected = height;
                    if ((s32)raw == expected) {
                        register s32 candidate asm("r6");
                        s32 x, z;
                        {
                            s32 width = SceneGrid_Width();
                            register s32 product asm("r0") = chosenRow;
                            product *= width;
                            candidate = chosenColumn + product;
                        }
                        x = Mod(candidate, SceneGrid_Width());
                        z = Div(candidate, SceneGrid_Width());
                        output->x = x * 256 + 128;
                        output->y = (*Properties(candidate) & 15) * 256;
                        output->z = z * 256 + 128;
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}
