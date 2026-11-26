#include "field/named_object/named_object.h"
#include "engine/shared/player.h"
#include "engine/shared/engine.h"
#include "field/named_object/named_object_flags.h"
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
void NamedObject_TryCapturePlayer(struct NamedObject *o)
{
    s32 i;
    i = 0;
    do {
        if (gPlayers[i] && Player_TestAttachmentFlags(gPlayers[i], 1) &&
            NamedObjectFlags_Has(8, &o->flags)) {
            s32 x = gPlayers[i]->position.x >> 8;
            s32 z = gPlayers[i]->position.z >> 8;
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
                    Difference(&delta, (struct SceneVec *)&o->body.position.x,
                               &gPlayers[i]->position);
                    if (delta.x > gUnk_086064B4.x || delta.x < -gUnk_086064B4.x ||
                        delta.y > gUnk_086064B4.y || delta.y < -gUnk_086064B4.y ||
                        delta.z > gUnk_086064B4.z || delta.z < -gUnk_086064B4.z) {
                        Player_SetPendingObject(gPlayers[i], &o->body.position.x);
                        NamedObject_EnableFlags(0x100, o);
                    }
                }
            }
        }
        i++;
    } while (i < 1);
}
