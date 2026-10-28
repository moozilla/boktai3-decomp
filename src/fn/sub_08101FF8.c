#include "named_object.h"
#include "player.h"
#include "engine.h"
extern const struct SceneVec gUnk_086064B4;
extern const struct SceneVec gUnk_086064BC;
s32 Player_TestAttachmentFlags(struct Player *, u32);
u8 NamedObject_IsMapCellOccupied(s32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
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
void NamedObject_UpdateEntryTrigger(struct NamedObject *o)
{
    s32 i;
    for (i = 0; i < 1; i++) {
        u32 outside = 0;
        if (gPlayers[i]) {
            if (!Player_TestAttachmentFlags(gPlayers[i], 1) &&
                !Player_TestAttachmentFlags(gPlayers[i], 2))
                return;
            if (HasFlag(8, &o->flags)) {
                s32 x = gPlayers[i]->position.x >> 8;
                s32 z = gPlayers[i]->position.z >> 8;
                s32 cell;
                u8 *data;
                u16 attr;
                if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
                    cell = 0;
                else
                    cell = gTerrainMap->rows[z] + x;
                data = TerrainModifiers_FindCellOverride(cell, 1);
                if (data)
                    data += 4;
                else
                    data = gTerrainMap->cells + (cell * 4 + 12);
                attr = *(u16 *)(data + 2);
                if (attr & 0x40) {
                    if (!NamedObject_IsMapCellOccupied(cell)) {
                        struct SceneVec delta;
                        Difference(&delta, (struct SceneVec *)&o->x, &gPlayers[i]->position);
                        if (o->field_E0 & 8) {
                            if (delta.x > gUnk_086064B4.x || delta.x < -gUnk_086064B4.x ||
                                delta.y > gUnk_086064B4.y || delta.y < -gUnk_086064B4.y ||
                                delta.z > gUnk_086064B4.z || delta.z < -gUnk_086064B4.z)
                                outside = 1;
                        } else {
                            if (delta.x > gUnk_086064BC.x || delta.x < -gUnk_086064BC.x ||
                                delta.y > gUnk_086064BC.y || delta.y < -gUnk_086064BC.y ||
                                delta.z > gUnk_086064BC.z || delta.z < -gUnk_086064BC.z)
                                outside = 1;
                        }
                        if (outside) {
                            if (o->enterScript)
                                Script_Call(o->enterScript, 0);
                        }
                    }
                }
            }
        }
    }
}
