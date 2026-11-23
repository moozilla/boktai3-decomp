#include "global.h"
#include "named_object_flags.h"

struct FollowObject {
    u8 pad0[0xB8];
    s16 flags;
    u8 padBA[0x158 - 0xBA];
    u8 cells[4][0x10];
    u8 registered[4];
};
void TerrainModifiers_UnregisterCell(void *);
void ActorGroup_ReleaseMember(struct FollowObject *o)
{
    s16 *flags;
    s32 i;
    if (NamedObjectFlags_Has(0x800, &o->flags)) {
        i = 0;
        flags = &o->flags;
        for (; i < 4; i++) {
            if (o->registered[i]) {
                TerrainModifiers_UnregisterCell(o->cells[i]);
                o->registered[i] = 0;
            }
        }
        NamedObjectFlags_Remove(0x800, flags);
    }
}
