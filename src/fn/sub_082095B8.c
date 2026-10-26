#define gTerrainMapWidth  gTerrainWidth
#define gTerrainMapHeight gTerrainHeight
#include "global.h"
#include "engine.h"
#include "scene_vec.h"

/* Tentative cannon view: distance controls sampling at one-cell intervals. */
struct FanCannon {
    u8 unknown0[0x2c];
    struct SceneVec position;
    u8 unknown34[0xd4];
    s32 distance;
};
union FanCannonSample {
    struct SceneVec position;
    struct {
        u32 x : 16;
        u32 y : 16;
        u32 z : 16;
    } packed;
};
s32 Div(s32, s32);
struct SceneVec *Player_GetEffectivePosition(s32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);

static inline void FanCannon_SubtractPositions(struct SceneVec *out, const struct SceneVec *a,
                                               const struct SceneVec *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}

static inline s32 FanCannon_GetCellIndex(struct SceneVec *position)
{
    s32 x = position->x >> 8;
    s32 z = position->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainMapWidth || (u32)z >= gTerrainMapHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

s32 FanCannon_IsPathToPlayerClear(struct FanCannon *cannon)
{
    struct SceneVec delta;
    union FanCannonSample sample;
    struct SceneVec *player;
    s32 steps, i;
    steps = Div(cannon->distance, 0x100);
    player = Player_GetEffectivePosition(0);
    FanCannon_SubtractPositions(&delta, player, &cannon->position);
    for (i = 1; i < steps; i++) {
        s32 cell;
        u8 *data;
        sample.packed.x = cannon->position.x + Div(i * delta.x, steps);
        sample.packed.y = cannon->position.y + 0x80;
        sample.packed.z = cannon->position.z + Div(i * delta.z, steps);
        cell = FanCannon_GetCellIndex(&sample.position);
        data = TerrainModifiers_FindCellOverride(cell, 1);
        if (data)
            data += 4;
        else
            data = gTerrainMap->cells + (cell * 4 + 12);
        if (((u32)(*data & 15) << 8) > cannon->position.y + 0x80)
            return 0;
    }
    return 1;
}
