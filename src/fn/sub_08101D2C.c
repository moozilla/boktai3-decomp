// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine.h"
#include "player.h"

#define TerrainRegistration_Remove  TerrainModifiers_UnregisterCell
#define TerrainRegistration_SetCell TerrainModifiers_RegisterCell
struct NamedObjectTerrainState {
    u8 unknown00[0xB6];
    u16 state;
    s16 flags;
    u8 unknownBA[0x10C - 0xBA];
    struct SceneVec position;
    u8 unknown114[0x148 - 0x114];
    u8 registration[0x10];
};
void TerrainModifiers_UnregisterCell(void *);
void TerrainModifiers_RegisterCell(void *, u32, u32, u32, u32, u32);
static inline u8 HasFlag(s32 mask, s16 *flags)
{
    if ((u16)*flags & mask)
        return TRUE;
    return FALSE;
}
static inline void ClearFlag(s32 mask, s16 *flags)
{
    *flags &= ~mask;
}
static inline void SetFlag(u16 *flags, u32 mask)
{
    u32 m = mask;
    asm("" : "+r"(m));
    *flags = m | *flags;
}
static inline s32 GetCell(struct SceneVec *position, struct EngineMap *map)
{
    s32 x, z, cell;
    x = position->x >> 8;
    z = position->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        cell = 0;
    else
        cell = map->rows[z] + x;
    return cell;
}
void NamedObject_UpdateTerrainRegistration(struct NamedObjectTerrainState *object)
{
    struct EngineMap *map;
    s32 cell;
    if ((u16)(object->state - 3) > 1) {
        if (HasFlag(1, &object->flags)) {
            TerrainModifiers_UnregisterCell(object->registration);
            ClearFlag(1, &object->flags);
        }
    } else {
        if (HasFlag(1, &object->flags))
            return;
        map = gTerrainMap;
        if (!map)
            return;
        if (object->position.y != gPlayers[0]->position.y)
            return;
        cell = GetCell(&object->position, map);
        TerrainModifiers_RegisterCell(object->registration, cell, 0, object->position.y >> 8, 0xFF,
                                      4);
        SetFlag((u16 *)&object->flags, 1);
    }
}
