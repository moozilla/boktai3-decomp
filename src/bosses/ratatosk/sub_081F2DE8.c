/* Tentative family and field names, based on static accesses. */
#include "global.h"
struct ProximityPosition {
    s16 x, y, z, unknown6;
};
struct ProximityEntry {
    struct ProximityPosition position;
    s32 squaredDistance;
    u8 unknownC[0xC];
};
struct ActorProximity {
    u8 unknown0[0x36C];
    struct ProximityPosition position;
    u8 unknown374[0x9A8];
    struct ProximityEntry entries[9];
};
void ActorProximity_UpdateSquaredDistances(struct ActorProximity *p)
{
    struct ProximityPosition *position = &p->position;
    s32 *distance = &p->entries[0].squaredDistance;
    s32 i = 8;
    struct ProximityEntry *entry = p->entries;
    do {
        s32 x = entry->position.x - position->x;
        s32 z = entry->position.z - position->z;
        *distance = x * x + z * z;
        distance = (s32 *)((u8 *)distance + 0x18);
        entry++;
        i--;
    } while (i >= 0);
}
