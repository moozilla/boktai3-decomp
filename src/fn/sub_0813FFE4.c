#include "global.h"
#include "engine.h"
#include "contact_actor.h"
void sub_0813FE30(void *);
void CellObject_Init(void *, void *);
void SceneBody_SetKeyframeDisplay(void *, void *);
void SceneBody_SetMovementProbe(void *, void *, s32, s32);
void SceneBody_SetPositionOffset(void *, void *);
u32 SceneActorContacts_RegisterNode(void *, void *, u32);
extern u16 gTrackedPlayerId[];
u8 *TerrainModifiers_FindCellOverride(s32, s32);
void SceneBody_Init(void *, u32, struct ContactPackedVector *, u32, u32, void *);
u32 Player_InitBodyFromScript(u8 *owner, s32 entry)
{
    struct ContactPackedVector position;
    struct ContactPackedVector offset;
    u8 *body;
    u8 *initialBody;
    u16 *point;
    s32 x, z;
    u32 cell;
    u8 *definition;
    u32 type;
    s32 height;
    s32 mode;
    u32 result;
    u32 id;
    if (Script_SeekToKeyword(0x70)) {
        position.x = Script_GetValue();
        position.y = Script_GetValue();
        position.z = Script_GetValue();
    } else {
        position.x = gGameState->positionX;
        position.y = gGameState->positionY;
        position.z = gGameState->positionZ;
    }
    point = (u16 *)&position;
    x = (s16)point[0] >> 8;
    z = (s16)point[2] >> 8;
    if (x < 0 || z < 0 || x >= gTerrainWidth || z >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[z] + x;
    definition = TerrainModifiers_FindCellOverride(cell, 1);
    if (definition)
        definition += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        definition = map->cells + offset;
    }
    type = *definition >> 4;
    height = (*definition & 15) << 8;
    switch (type) {
        case 1:
            height -= (u8)point[2];
            break;
        case 2:
            height -= (u8)point[0];
            break;
    }
    ((u16 *)&position)[1] = height;
    initialBody = owner + 0x24;
    SceneBody_Init(initialBody, entry, &position, 0, owner[0x18], owner);
    sub_0813FE30(owner);
    mode = owner[0x418];
    body = initialBody;
    switch (mode) {
        case 0:
        case 1:
        case 2:
        case 6:
            owner[0x419] = 0;
            SceneBody_SetKeyframeDisplay(body, owner + 0x8c);
            break;
    }
    CellObject_Init(owner + 0x284, &position);
    SceneBody_SetMovementProbe(body, owner + 0x284, 30, 30);
    offset.x = 0;
    offset.y = 300;
    offset.z = 0;
    SceneBody_SetPositionOffset(body, &offset);
    id = *(u32 *)(owner + 0x24);
    *(u16 *)(owner + 0x294) = id;
    result = SceneActorContacts_RegisterNode(owner + 0x2e8, body, 2);
    gTrackedPlayerId[owner[0x2c]] = *(u32 *)(owner + 0x24);
    return result;
}
