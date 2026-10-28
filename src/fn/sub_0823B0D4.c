#include "global.h"
#include "ram.h"

/* tentative: reads the script position and sets up the body, probe and contact node */

#include "engine.h"

struct Vec16 {
    s16 x, y, z;
};

u8 *TerrainModifiers_FindCellOverride(s32, s32);
u32 SceneBody_Init(void *, u32, void *, u32, u32, void *);
void sub_0823AE5C(u8 *);
void KeyframeActor_InitAuxiliarySprite(u8 *);
u32 SceneBody_SetKeyframeDisplay(void *, void *);
void CellObject_Init(void *, void *);
void SceneBody_SetMovementProbe(void *, void *, s32, s32);
u32 SceneBody_SetPositionOffset(void *, struct Vec16 *);
u32 SceneActorContacts_RegisterNode(void *, void *, u32);

u32 KeyframeActor_InitBodyFromScript(u8 *p, u32 entry)
{
    s32 found = Script_SeekToKeyword('p');
    struct Vec16 *pos;
    s32 x, z, cell;
    u8 *prop;
    u32 surface, height;
    u8 *body;
    struct Vec16 off;
    u32 zero;
    u32 id;

    if (found) {
        *(s16 *)(p + 0x540) = Script_GetValue();
        *(s16 *)(p + 0x542) = Script_GetValue();
        *(s16 *)(p + 0x544) = Script_GetValue();
    } else {
        *(s16 *)(p + 0x540) = found;
        *(s16 *)(p + 0x542) = found;
        *(s16 *)(p + 0x544) = found;
    }
    pos = (struct Vec16 *)(p + 0x540);
    x = pos->x >> 8;
    z = pos->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        cell = 0;
    else {
        u32 row = gTerrainMap->rows[z];
        cell = row + x;
    }
    prop = TerrainModifiers_FindCellOverride(cell, 1);
    if (prop)
        prop += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        prop = map->cells + offset;
    }
    surface = *prop >> 4;
    height = (*prop & 15) << 8;
    switch (surface) {
        case 1:
            height -= (u8)pos->z;
            break;
        case 2:
            height -= (u8)pos->x;
            break;
    }
    {
        s16 *h = (s16 *)(p + 0x542);
        zero = 0;
        *h = height;
    }
    body = p + 0x24;
    SceneBody_Init(body, entry, p + 0x540, 0, p[0x18], p);
    sub_0823AE5C(p);
    KeyframeActor_InitAuxiliarySprite(p);
    p[0x419] = zero;
    SceneBody_SetKeyframeDisplay(body, p + 0x8c);
    CellObject_Init(p + 0x284, p + 0x540);
    SceneBody_SetMovementProbe(body, p + 0x284, 0x1e, 0x1e);
    off.x = 0;
    off.y = 0x12c;
    off.z = 0;
    SceneBody_SetPositionOffset(body, &off);
    id = *(u32 *)(p + 0x24);
    *(u16 *)(p + 0x294) = id;
    SceneActorContacts_RegisterNode(p + 0x2e8, body, 2);
    gTrackedPlayerId[*(u32 *)(p + 0x18)] = *(u32 *)(p + 0x24);
}
