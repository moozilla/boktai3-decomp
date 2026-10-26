#define Player_TestTerrainRecoveryEnabled Player_IsStatusBit4ActionActive
#include "global.h"
#include "player.h"
#include "engine.h"
u8 *TerrainModifiers_FindCellOverride(s32, s32);
u32 Player_IsStatusBit4ActionActive(u32);
u32 NamedObject_FindNearPosition(struct SceneVec *);
s32 Mod(s32, s32);
s32 Div(s32, s32);
void Player_ClampPositionToStoredTerrainCell(struct Player *p)
{
    s32 x = p->position.x >> 8;
    s32 z = p->position.z >> 8;
    s32 cell;
    u8 *properties;
    s32 cellX, cellZ, minX, maxX, minZ, maxZ;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[z] + x;
    properties = TerrainModifiers_FindCellOverride(cell, 1);
    if (properties)
        properties += 4;
    else
        properties = gTerrainMap->cells + (cell * 4 + 12);
    if (!Player_IsStatusBit4ActionActive(*(u16 *)(properties + 2)))
        return;
    if (p->tetherStatusFlags)
        return;
    if (p->terrainClampBlocked)
        return;
    if (NamedObject_FindNearPosition(&p->position))
        return;
    cellX = Mod(p->storedTerrainCell, *(s16 *)(gTerrainMap->cells + 4));
    cellZ = Div(p->storedTerrainCell, *(s16 *)(gTerrainMap->cells + 4));
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
