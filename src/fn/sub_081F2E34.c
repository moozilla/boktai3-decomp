#include "global.h"
#include "scene_vec.h"

#define Ratatosk_UpdatePointDistancesToPlayer sub_081F2E34
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
    u32 offset = 0;
    s32 count = 8;
    do {
        struct SceneVec *target = Player_GetEffectivePosition(0);
        s32 x = entry->position.x - target->x;
        s32 z = entry->position.z - target->z;
        x *= x;
        x += z * z;
        {
            u8 *base = (u8 *)&set->entries[0].distanceSquared;
            *(s32 *)(base + offset) = x;
        }
        entry++;
        offset += 24;
    } while (--count >= 0);
}