// MATCHING_CONCESSION: register-pinning
#include "scene_grid.h"
#include "contact_actor.h"
s32 Div(s32, s32);
s32 Mod(s32, s32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
static inline void FillResolved(struct ContactPosition *position, s32 index)
{
    s32 column, row;
    u8 *properties;
    column = Mod(index, SceneGrid_Width());
    row = Div(index, SceneGrid_Width());
    position->x = column * 256 + 128;
    properties = TerrainModifiers_FindCellOverride(index, 1);
    if (properties)
        properties += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = index * 4 + 12;
        properties = map->cells + offset;
    }
    position->y = (*properties & 15) * 256;
    position->z = row * 256 + 128;
}
static inline void Fill(struct ContactPosition *position, s32 index)
{
    if (index < 0)
        index = -index;
    FillResolved(position, index);
}

s32 SceneGrid_ResolveEndpoint(struct ContactPosition *first, struct ContactPosition *second,
                              s32 firstIndex, s32 secondIndex)
{
    if (secondIndex == -255) {
        if (firstIndex == secondIndex) {
            s32 width = SceneGrid_Width();
            register s32 scaled asm("r0") = width * 12;
            s32 fallback;
            fallback = scaled + 10;
            {
                register s32 index asm("r5") = fallback;
                register s32 incoming asm("r2") = fallback;
                if (incoming < 0)
                    index = -incoming;
                FillResolved(second, index);
            }
        } else {
            Fill(first, firstIndex);
            return 1;
        }
    } else {
        Fill(first, secondIndex);
    }
    return -1;
}
