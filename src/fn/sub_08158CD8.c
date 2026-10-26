#include "global.h"
#include "player.h"
#include "engine.h"
u8 *TerrainModifiers_FindCellOverride(s32, s32);
void ActorSequence_Begin(struct Player *, s32, s32);
static inline s32 cellIndex(s32 x, s32 z)
{
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline u8 *cellProperties(s32 cell)
{
    u8 *properties = TerrainModifiers_FindCellOverride(cell, 1);
    if (properties)
        properties += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        properties = map->cells + offset;
    }
    return properties;
}
void Player_BeginTerrainInteraction(struct Player *p)
{
    struct SceneVec *position;
    u8 *properties;
    u32 surface;
    s32 height;
    if (p->actionMode == 5) {
        p->actionLatch464 = 1;
        p->actionLatch465 = 1;
    }
    ActorSequence_Begin(p, 0x1C, 0);
    p->terrainRecoveryCounter = 0;
    position = &p->position;
    properties = cellProperties(cellIndex(position->x >> 8, position->z >> 8));
    surface = *properties >> 4;
    height = (*properties & 15) << 8;
    switch (surface) {
        case 1:
            height -= (u8)position->z;
            break;
        case 2:
            height -= (u8)position->x;
            break;
    }
    p->actionCountdown = p->position.y - height;
    if (p->facing > 4) {
        p->heading = 8 - p->facing;
        p->side = 1;
    } else {
        p->heading = p->facing;
        p->side = 0;
    }
    p->directionFrame = 0;
}
