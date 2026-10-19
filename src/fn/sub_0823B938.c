#define Player_TestTerrainRecoveryEnabled Player_IsFlaggedWithGlobalModifier10
#include "global.h"
/* Player_ClampPositionToStoredTerrainCell_0823B938: same code as Player_ClampPositionToStoredTerrainCell at another address (ported by relatives.py; provisional name) */
#define Player_ClampPositionToStoredTerrainCell_0823B938 sub_0823B938
#include "player.h"
#include "engine.h"
u8 *TerrainModifiers_FindCellOverride(s32, s32);
u32 Player_IsFlaggedWithGlobalModifier10(u32);
u32 NamedObject_FindNearPosition(struct SceneVec *);
s32 Mod(s32, s32);
s32 Div(s32, s32);
void Player_ClampPositionToStoredTerrainCell_0823B938(struct Player *p)
{
    s32 x = p->position.x >> 8;
    s32 z = p->position.z >> 8;
    s32 cell;
    u8 *properties;
    s32 cellX, cellZ, minX, maxX, minZ, maxZ;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        cell = 0;
    else
        cell = gUnk_030052F4->rows[z] + x;
    properties = TerrainModifiers_FindCellOverride(cell, 1);
    if (properties)
        properties += 4;
    else
        properties = gUnk_030052F4->cells + (cell * 4 + 12);
    if (!Player_IsFlaggedWithGlobalModifier10(*(u16 *)(properties + 2)))
        return;
    if (p->tetherStatusFlags)
        return;
    if (p->terrainClampBlocked)
        return;
    if (NamedObject_FindNearPosition(&p->position))
        return;
    cellX = Mod(p->storedTerrainCell, *(s16 *)(gUnk_030052F4->cells + 4));
    cellZ = Div(p->storedTerrainCell, *(s16 *)(gUnk_030052F4->cells + 4));
    cellX <<= 8;
    p->terrainCellCenter.x = cellX + 128;
    cellZ <<= 8;
    p->terrainCellCenter.z = cellZ + 128;
    p->terrainTarget = &p->terrainCellCenter;
    minX = cellX + 1;
    maxX = cellX + 255;
    minZ = cellZ + 1;
    maxZ = cellZ + 255;
    if (p->position.x < minX)
        p->position.x = minX;
    else if (p->position.x > maxX)
        p->position.x = maxX;
    if (p->position.z < minZ)
        p->position.z = minZ;
    else if (p->position.z > maxZ)
        p->position.z = maxZ;
    p->colliderPosition = p->position;
    p->previousPosition = p->position;
    p->terrainClampApplied = 1;
}
