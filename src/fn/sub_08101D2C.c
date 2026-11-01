#include "global.h"
#include "named_object.h"
#include "player.h"

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
    m |= *flags;
    *flags = m;
}
static inline s32 GetCell(struct ContactPosition *position, struct EngineMap *map)
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
void NamedObject_UpdateTerrainRegistration(struct NamedObject *object)
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
        if (object->body.position.y != gPlayers[0]->position.y)
            return;
        cell = GetCell(&object->body.position, map);
        TerrainModifiers_RegisterCell(object->registration, cell, 0, object->body.position.y >> 8,
                                      0xFF, 4);
        SetFlag((u16 *)&object->flags, 1);
    }
}
