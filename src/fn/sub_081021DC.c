#include "named_object.h"
#include "player.h"
#include "engine.h"
extern const struct SceneVec gUnk_086064C4;
s32 Player_TestAttachmentFlags(struct Player *, u32);
void Player_SetPositionAndSave(struct Player *, struct SceneVec *);
u8 NamedObject_IsMapCellOccupied(s32);
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
void NamedObject_ConstrainPlayer(struct NamedObject *o)
{
    struct SceneVec delta;
    s32 i;
    for (i = 0; i < 1; i++) {
        if (gUnk_02000580[i]) {
            if ((s16)o->state != 4 && Player_TestAttachmentFlags(gUnk_02000580[i], 1)) {
                s32 x, z, cell;
                if (!HasFlag(8, &o->flags))
                    return;
                x = ((((struct SceneVec *)&o->x)->x >> 4) >> 2) >> 2;
                z = ((struct SceneVec *)&o->x)->z >> 8;
                if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
                    cell = 0;
                else
                    cell = gTerrainMap->rows[z] + x;
                if (!(NamedObject_IsMapCellOccupied(cell) &&
                      gUnk_02000580[i]->position.y == o->y)) {
                    Difference(&delta, (struct SceneVec *)&o->x, &gUnk_02000580[i]->position);
                    if (delta.x > gUnk_086064C4.x || delta.x < -gUnk_086064C4.x ||
                        delta.y > gUnk_086064C4.y || delta.y < -gUnk_086064C4.y ||
                        delta.z > gUnk_086064C4.z || delta.z < -gUnk_086064C4.z) {
                        if (delta.x > gUnk_086064C4.x)
                            delta.x = gUnk_086064C4.x;
                        else if (delta.x < -gUnk_086064C4.x)
                            delta.x = -gUnk_086064C4.x;
                        if (delta.z > gUnk_086064C4.z)
                            delta.z = gUnk_086064C4.z;
                        else if (delta.z < -gUnk_086064C4.z)
                            delta.z = -gUnk_086064C4.z;
                        delta.x = o->x + delta.x;
                        delta.y = o->y;
                        delta.z = o->z + delta.z;
                        Player_SetPositionAndSave(gUnk_02000580[i], &delta);
                        SetFlag2(0x100, (u16 *)&o->flags);
                    }
                }
            }
        }
    }
}
