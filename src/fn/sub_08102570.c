#include "named_object.h"
#include "player.h"
#include "engine.h"
extern const struct SceneVec gUnk_086064B4;
s32 TetherActor_TestStatusFlags(u32);
void TetherActor_CopyPosition(struct SceneVec *);
void TetherActor_SetPosition(struct SceneVec *);
u8 NamedObject_IsMapCellOccupied(s32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
static inline u8 *CellData(s32 cell)
{
    u8 *p = TerrainModifiers_FindCellOverride(cell, 1);
    if (p)
        return p + 4;
    return gTerrainMap->cells + (cell * 4 + 12);
}
static inline u32 Attributes(s32 cell)
{
    return *(u16 *)(CellData(cell) + 2);
}
static inline void Difference(struct SceneVec *d, const struct SceneVec *b,
                              const struct SceneVec *a)
{
    d->x = a->x - b->x;
    d->y = a->y - b->y;
    d->z = a->z - b->z;
}
static inline u8 HasFlag(u32 mask, s16 *flags)
{
    if ((u16)*flags & mask)
        return TRUE;
    return FALSE;
}
static inline void SetFlag2(u16 mask, u16 *flags)
{
    *flags = mask | *flags;
}
void NamedObject_ClampTetherAtBarrier(struct NamedObject *o)
{
    struct SceneVec tether;
    struct SceneVec delta;
    s32 x, z, cell;
    if (!TetherActor_TestStatusFlags(1))
        return;
    if (!HasFlag(0x10, &o->flags))
        return;
    TetherActor_CopyPosition(&tether);
    x = tether.x >> 8;
    z = tether.z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[z] + x;
    if (!(Attributes(cell) & 0x80))
        return;
    Difference(&delta, (struct SceneVec *)&o->body.position.x, &tether);
    if (delta.x > gUnk_086064B4.x || delta.x < -gUnk_086064B4.x || delta.y > gUnk_086064B4.y ||
        delta.y < -gUnk_086064B4.y || delta.z > gUnk_086064B4.z || delta.z < -gUnk_086064B4.z) {
        if (delta.x > gUnk_086064B4.x)
            delta.x = gUnk_086064B4.x;
        else if (delta.x < -gUnk_086064B4.x)
            delta.x = -gUnk_086064B4.x;
        if (delta.z > gUnk_086064B4.z)
            delta.z = gUnk_086064B4.z;
        else if (delta.z < -gUnk_086064B4.z)
            delta.z = -gUnk_086064B4.z;
        tether.x = o->body.position.x + delta.x;
        tether.y = o->body.position.y;
        tether.z = o->body.position.z + delta.z;
        TetherActor_SetPosition(&tether);
        SetFlag2(0x200, (u16 *)&o->flags);
    }
}
