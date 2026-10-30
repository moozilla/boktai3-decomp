#ifndef GUARD_SCENE_ACTOR_INIT_PRIMITIVES_H
#define GUARD_SCENE_ACTOR_INIT_PRIMITIVES_H

#include "global.h"
#include "engine_globals.h"
#include "engine_map.h"
#include "contact_position.h"

s32 Script_GetValue(void);
u8 *Script_GetPc(void);
s32 Script_SeekToKeyword(s32 keyword);
s32 Script_GetKeywordValue(s32 keyword, s32 fallback);
u8 *TerrainModifiers_FindCellOverride(s32 cell, s32 kind);
void CpuSet(const void *, void *, u32);

/* Optional operands have separate arithmetic, byte and halfword domains. */
static inline s32 MonsterInit_ReadWord(s32 fallback)
{
    if (Script_GetPc())
        return Script_GetValue();
    return fallback;
}

static inline u8 MonsterInit_ReadByte(u8 fallback)
{
    if (Script_GetPc())
        return Script_GetValue();
    return fallback;
}

static inline u16 MonsterInit_ReadHalfword(u16 fallback)
{
    if (Script_GetPc())
        return Script_GetValue();
    return fallback;
}

static inline u16 MonsterInit_ReadKeywordHalfword(u32 keyword, s32 fallback)
{
    return Script_GetKeywordValue(keyword, fallback);
}

static inline void MonsterInit_ReadScriptWord(u32 *output, u32 fallback)
{
    *output = MonsterInit_ReadWord(fallback);
}

static inline void MonsterInit_ClearPosition(u32 *fill, void *object)
{
    struct ContactPosition *position = object;
    *fill = 0;
    CpuSet(fill, position, 0x05000002);
}

/* Coordinates keep their stored halfword domain until the signed cell calculation. */
static inline u16 MonsterInit_PositionXRaw(const struct ContactPosition *position)
{
    return position->x;
}

static inline u16 MonsterInit_PositionZRaw(const struct ContactPosition *position)
{
    return position->z;
}

static inline u32 MonsterInit_CellIndex(const struct ContactPosition *position)
{
    s32 x = (s16)MonsterInit_PositionXRaw(position) >> 8;
    s32 z = (s16)MonsterInit_PositionZRaw(position) >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

/* Actor cell IDs are stored as halfwords; map calculations use a word index. */
static inline u16 MonsterInit_CellId(const struct ContactPosition *position)
{
    return MonsterInit_CellIndex(position);
}

static inline u8 MonsterInit_TerrainShape(const u8 *cell)
{
    return *cell >> 4;
}

static inline u32 MonsterInit_TerrainBaseHeight(const u8 *cell)
{
    return (*cell & 15) << 8;
}

static inline s32 MonsterInit_TerrainHeight(s32 cell, const struct ContactPosition *position)
{
    u8 *data = TerrainModifiers_FindCellOverride(cell, 1);
    u32 shape;
    s32 height;
    if (data)
        data += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        data = map->cells + offset;
    }
    shape = MonsterInit_TerrainShape(data);
    height = MonsterInit_TerrainBaseHeight(data);
    switch (shape) {
        case 1:
            height -= ((const u8 *)position)[4];
            break;
        case 2:
            height -= ((const u8 *)position)[0];
            break;
    }
    return height;
}

#endif
