#include "named_object.h"
#include "player.h"
#include "engine.h"
#include "named_object_flags.h"
extern const struct SceneVec gUnk_086064C4;
s32 TetherActor_TestStatusFlags(u32);
void TetherActor_CopyPosition(struct SceneVec *);
void TetherActor_SetPosition(struct SceneVec *);
u8 NamedObject_IsMapCellOccupied(s32);
static inline void Difference(struct SceneVec *d, const struct SceneVec *b,
                              const struct SceneVec *a)
{
    d->x = a->x - b->x;
    d->y = a->y - b->y;
    d->z = a->z - b->z;
}
void NamedObject_ConstrainTether(struct NamedObject *o)
{
    struct SceneVec tether;
    struct SceneVec delta;
    s32 x, z, cell;
    if (!TetherActor_TestStatusFlags(1))
        return;
    if (!NamedObjectFlags_Has(0x10, &o->flags))
        return;
    TetherActor_CopyPosition(&tether);
    x = ((struct SceneVec *)&o->body.position.x)->x >> 8;
    z = ((struct SceneVec *)&o->body.position.x)->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[z] + x;
    if (NamedObject_IsMapCellOccupied(cell) && tether.y == o->body.position.y)
        return;
    Difference(&delta, (struct SceneVec *)&o->body.position.x, &tether);
    if (delta.x > gUnk_086064C4.x || delta.x < -gUnk_086064C4.x || delta.y > gUnk_086064C4.y ||
        delta.y < -gUnk_086064C4.y || delta.z > gUnk_086064C4.z || delta.z < -gUnk_086064C4.z) {
        if (delta.x > gUnk_086064C4.x)
            delta.x = gUnk_086064C4.x;
        else if (delta.x < -gUnk_086064C4.x)
            delta.x = -gUnk_086064C4.x;
        if (delta.z > gUnk_086064C4.z)
            delta.z = gUnk_086064C4.z;
        else if (delta.z < -gUnk_086064C4.z)
            delta.z = -gUnk_086064C4.z;
        tether.x = o->body.position.x + delta.x;
        tether.y = o->body.position.y;
        tether.z = o->body.position.z + delta.z;
        TetherActor_SetPosition(&tether);
        NamedObject_EnableFlags(0x200, o);
    }
}
