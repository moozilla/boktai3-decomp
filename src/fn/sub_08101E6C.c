#include "named_object.h"
#include "player.h"
#include "engine.h"
extern const struct SceneVec gUnk_086064B4;
static inline void Difference(struct SceneVec *d, const struct SceneVec *b,
                              const struct SceneVec *a)
{
    d->x = a->x - b->x;
    d->y = a->y - b->y;
    d->z = a->z - b->z;
}
s32 Player_TestAttachmentFlags(struct Player *, u32);
void Player_SetPendingObject(struct Player *, void *);
u8 NamedObject_IsMapCellOccupied(s32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
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
void NamedObject_TryCapturePlayer(struct NamedObject *o)
{
    s32 i;
    i = 0;
    do {
        if (gUnk_02000580[i] && Player_TestAttachmentFlags(gUnk_02000580[i], 1) &&
            HasFlag(8, &o->flags)) {
            s32 x = gUnk_02000580[i]->position.x >> 8;
            s32 z = gUnk_02000580[i]->position.z >> 8;
            s32 cell;
            u8 *data;
            u16 attr;
            if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
                cell = 0;
            else
                cell = gTerrainMap->rows[z] + x;
            if (!NamedObject_IsMapCellOccupied(cell)) {
                data = TerrainModifiers_FindCellOverride(cell, 1);
                if (data)
                    data += 4;
                else
                    data = gTerrainMap->cells + (cell * 4 + 12);
                attr = *(u16 *)(data + 2);
                if (attr & 0x80) {
                    struct SceneVec delta;
                    Difference(&delta, (struct SceneVec *)&o->x, &gUnk_02000580[i]->position);
                    if (delta.x > gUnk_086064B4.x || delta.x < -gUnk_086064B4.x ||
                        delta.y > gUnk_086064B4.y || delta.y < -gUnk_086064B4.y ||
                        delta.z > gUnk_086064B4.z || delta.z < -gUnk_086064B4.z) {
                        Player_SetPendingObject(gUnk_02000580[i], &o->x);
                        SetFlag2(0x100, (u16 *)&o->flags);
                    }
                }
            }
        }
        i++;
    } while (i < 1);
}
