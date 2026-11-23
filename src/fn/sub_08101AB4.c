#include "global.h"
#include "named_object_flags.h"
#include "named_object.h"

void ActorGroup_ReleaseMember(struct NamedObject *o)
{
    s16 *flags;
    s32 i;
    if (NamedObjectFlags_Has(0x800, &o->flags)) {
        i = 0;
        flags = &o->flags;
        for (; i < 4; i++) {
            if (o->cellRegistered[i]) {
                TerrainModifiers_UnregisterCell(o->cellRegistrations[i]);
                o->cellRegistered[i] = 0;
            }
        }
        NamedObjectFlags_Remove(0x800, flags);
    }
}
