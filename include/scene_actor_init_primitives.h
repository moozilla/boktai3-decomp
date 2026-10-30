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

#endif
