#include "global.h"
#include "engine.h"
#include "player.h"
#include "scene_grid.h"

// confidence=low: limits the tether target distance and clips its position against terrain.

u8 *TerrainModifiers_FindCellOverride(s32, s32);
u32 TetherActor_GetStatusFlags(void);
struct SceneVec *TetherActor_GetPosition(void);
static inline u8 *properties(struct SceneVec *position)
{
    s32 x = position->x >> 8, z = position->z >> 8;
    s32 cell;
    u8 *p;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[z] + x;
    p = TerrainModifiers_FindCellOverride(cell, 1);
    if (p)
        p += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        p = map->cells + offset;
    }
    return p;
}
struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
u32 Player_TestTetherTarget(void *, void *);
void TetherActor_SetInteractionTarget(void *, u32);
static inline s32 scale(s32 v)
{
    s32 result;
    if (v >= 0)
        result = v >> 12;
    else
        result = -((-v) >> 12);
    return result;
}
static inline s32 indexAt(struct SceneVec *position)
{
    s32 x = position->x >> 8, z = position->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline s32 heightAt(struct SceneVec *position)
{
    u8 *p = properties(position);
    s32 surface = *p >> 4, height = (*p & 15) << 8;
    switch (surface) {
        case 1:
            height -= (u8)position->z;
            break;
        case 2:
            height -= (u8)position->x;
            break;
    }
    return height;
}
void Player_ConstrainTetherTarget(struct Player *actor, s32 dx, s32 dz, s32 radius)
{
    s32 ox = scale(70 * dx);
    s32 oz = scale(70 * dz);
    struct SceneVec *target = TetherActor_GetPosition();
    s32 x = (target->x - actor->position.x) - ox;
    s32 z = (target->z - actor->position.z) - oz;
    if (((x * x) + (z * z)) > (radius * radius)) {
        s32 angle = Math_Atan2(x, z);
        struct PackedVec candidate;
        candidate.x = (actor->position.x + scale(gUnk_086149C4[(angle + 64) & 255] * radius)) + ox;
        candidate.z = (actor->position.z + scale(gUnk_086149C4[angle & 255] * radius)) + oz;
        candidate.x = ((target->x * 3) + candidate.x) >> 2;
        candidate.z = ((target->z * 3) + candidate.z) >> 2;
        if (!Player_TestTetherTarget(actor, &candidate)) {
            u16 rawX = actor->position.x;
            s32 cellx = ((s16)rawX) >> 8;
            struct SceneVec *position = &actor->position;
            s32 cellz = position->z >> 8;
            s32 current;
            s32 next;
            u16 ax = rawX;
            if ((((cellx < 0) || (cellz < 0)) || (((u32)cellx) >= gTerrainWidth)) ||
                (((u32)cellz) >= gTerrainHeight))
                current = 0;
            else
                current = gTerrainMap->rows[cellz] + cellx;
            next = indexAt((void *)(&candidate));
            if (current == (next + 1))
                candidate.x = ((((s16)ax) >> 8) << 8) + 2;
            else if (current == (next - 1))
                candidate.x = (((((s16)ax) >> 8) + 1) << 8) - 2;
            else {
                s32 width = SceneGrid_Width();
                if (current == (next + width))
                    candidate.z = ((actor->position.z >> 8) << 8) + 2;
                else if (current == (next - width))
                    candidate.z = (((actor->position.z >> 8) + 1) << 8) - 2;
                else {
                    candidate.x = ax;
                    candidate.z = actor->position.z;
                }
            }
            if (!TetherActor_GetStatusFlags())
                candidate.y = heightAt((void *)(&candidate));
        } else if (!TetherActor_GetStatusFlags())
            candidate.y = heightAt((void *)(&candidate));
        x = actor->position.x - candidate.x;
        z = actor->position.z - candidate.z;
        angle = (u16)Math_Atan2(x, z);
        TetherActor_SetInteractionTarget(&candidate, ((((u16)angle) + 0x70) >> 5) & 7);
    }
}
