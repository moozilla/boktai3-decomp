#include "global.h"
#include "engine/ram.h"
#include "player/player.h"
#include "engine/engine.h"
#include "scene/collision/contact_actor.h"
void sub_0813FE30(struct Player *);
void CellObject_Init(void *, void *);
void SceneBody_SetKeyframeDisplay(void *, void *);
void SceneBody_SetMovementProbe(void *, void *, s32, s32);
void SceneBody_SetPositionOffset(void *, void *);
u32 SceneActorContacts_RegisterNode(void *, void *, u32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
void SceneBody_Init(void *, u32, struct ContactPackedVector *, u32, u32, void *);
u32 Player_InitBodyFromScript(struct Player *p, s32 entry)
{
    struct ContactPackedVector position;
    struct ContactPackedVector offset;
    void *body;
    void *initialBody;
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
    initialBody = &p->positionalEffectGroup;
    SceneBody_Init(initialBody, entry, &position, 0, (u8)p->slot, p);
    sub_0813FE30(p);
    mode = p->actionMode;
    body = initialBody;
    switch (mode) {
        case 0:
        case 1:
        case 2:
        case 6:
            p->unk419 = 0;
            SceneBody_SetKeyframeDisplay(body, &p->animation);
            break;
    }
    CellObject_Init(&p->movementContactFlags, &position);
    SceneBody_SetMovementProbe(body, &p->movementContactFlags, 30, 30);
    offset.x = 0;
    offset.y = 300;
    offset.z = 0;
    SceneBody_SetPositionOffset(body, &offset);
    id = *(u32 *)&p->positionalEffectGroup;
    *(u16 *)p->positionTracker = id;
    result = SceneActorContacts_RegisterNode(p->contactListNode, body, 2);
    gTrackedPlayerId[p->movementRequestBlocked] = *(u32 *)&p->positionalEffectGroup;
    return result;
}
