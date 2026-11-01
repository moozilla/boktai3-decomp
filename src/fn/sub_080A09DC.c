#include "scene_actor_script.h"
#include "engine.h"

struct Pair8 {
    u32 a, b;
};
struct CrowSpawnState {
    u8 unknown0[0x59];
    u8 option59;
    u8 unknown5A[0x3D4 - 0x5A];
    u32 flags;
    u8 unknown3D8;
    u8 distance;
    u16 angleOffset;
    u8 option;
    u8 unknown3DD;
    u16 groundHeight;
    u32 radiusSquared;
    u8 unknown3E4[4];
    u16 cell;
    u16 heightOffset;
    u8 unknown3EC[0x434 - 0x3EC];
    struct Pair8 position;
};
static inline s32 ReadOrZero(void)
{
    if (Script_GetPc())
        return Script_GetValue();
    return 0;
}
static inline u32 CellAtPosition(struct ContactPosition *position)
{
    s32 x = position->x >> 8;
    s32 z = position->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
inline static s32 Crow_GetGroundHeight(u16 cell, struct ContactPosition *position)
{
    u8 *override = TerrainModifiers_FindCellOverride(cell, 1);
    u8 *data;
    u32 type;
    s32 height;
    if (override)
        data = override + 4;
    else
        data = (gTerrainMap->cells + 12) + (cell * 4);
    height = data[0];
    type = (u32)height >> 4;
    {
        u32 level = 15;
        level &= height;
        height = level << 8;
    }
    {
        u32 slope;
        if (type == 1)
            goto alongZ;
        if (type == 2)
            goto alongX;
        goto finished;
    alongZ:
        slope = (u8)position->z;

        goto subtract;
    alongX:
        slope = (u8)position->x;

    subtract:
        height -= slope;
    }
finished:
    return height;
}

void Crow_ReadSpawnScript(void *input)
{
    struct SceneActorScriptPositionView *actor = input;
    struct CrowSpawnState *s = input;
    if (Script_SeekToKeyword(0x50)) {
        u32 value;
        u32 distance;
        u32 offset;
        {
            u32 *radius = &(s->radiusSquared);
            *radius = ReadOrZero();
        }
        s->option = ReadOrZero();
        distance = ReadOrZero();
        s->distance = distance;
        if (((u8)distance) <= 0x19)
            s->distance = 0x1A;
        gRandomIndex = (gRandomIndex + 1) & 0x3FF;
        value = *((const u16 *)((gRandomIndex * 2) + ((u32)gRandomTable)));
        value &= 0x3F;
        if (Script_GetPc())
            offset = Script_GetValue() + value;
        else
            offset = value;
        s->angleOffset = offset;
        if (Script_GetPc() && Script_GetValue()) {
            u32 mask = 4;
            u32 *flags = &(s->flags);
            *flags |= mask;
        }
        {
            u32 *radius = &(s->radiusSquared);
            *radius = (*radius) * (*radius);
        }
    }
    ((struct CrowSpawnState *)actor)->option59 = Script_GetKeywordValue(0x64, 0);
    if (Script_SeekToKeyword(0x70)) {
        u16 *height;
        struct Pair8 *copy;
        actor->position.x = ReadOrZero();
        actor->position.y = ReadOrZero();
        {
            u8 *pc = Script_GetPc();
            s32 value;
            height = &actor->position.y;
            if (pc)
                value = Script_GetValue();
            else
                value = 0;
            actor->position.z = value;
        }
        copy = &s->position;
        *copy = *((struct Pair8 *)(&actor->position));
        actor->cellIndex = CellAtPosition((struct ContactPosition *)&s->position);
        {
            u32 cell = actor->cellIndex;
            s32 gh;
            gh = Crow_GetGroundHeight(cell, (void *)&s->position);
            actor->groundHeight = gh;
        }
        {
            u16 y = *height;
            actor->heightOffset = y;
            s->heightOffset = y;
        }
        *height = actor->groundHeight;
        {
            u16 cell = actor->cellIndex;
            s->cell = cell;
        }
        {
            u16 gh = actor->groundHeight;
            s->groundHeight = gh;
        }
    }
}
