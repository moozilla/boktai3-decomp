// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_vec.h"

#ifdef NONMATCHING
// fuzzy: 97.6%
// Remaining difference: loop initializers are emitted in reverse order.
#define SceneDistance_UpdateSquaredDistances Ratatosk_UpdatePointDistancesToPlayer
struct SceneDistanceEntry {
    struct SceneVec position;
    u8 pad8[4];
    s32 distanceSquared;
    u8 pad10[8];
};
struct SceneDistanceSet {
    u8 pad0[0xD1C];
    struct SceneDistanceEntry entries[9];
};
struct SceneVec *Player_GetEffectivePosition(s32);
void Ratatosk_UpdatePointDistancesToPlayer(struct SceneDistanceSet *set)
{
    struct SceneDistanceEntry *entry = set->entries;
    register s32 count asm("r6") = 8;
    u32 offset = 0;
    do {
        struct SceneVec *target = Player_GetEffectivePosition(0);
        s32 x = entry->position.x - target->x;
        s32 z = entry->position.z - target->z;
        x *= x;
        x += z * z;
        set->entries[offset].distanceSquared = x;
        entry++;
        offset++;
    } while (--count >= 0);
}
#else
INCLUDE_ASM("asm/nonmatching", Ratatosk_UpdatePointDistancesToPlayer);
#endif
