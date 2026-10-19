#include "global.h"

#define GrowthHazard_FindNearestActiveEntry sub_081B7568

struct GrowthHazardPosition {
    s16 x, y, z, unk6;
};
struct GrowthHazardEntry {
    u8 unk0[0xE4];
    struct GrowthHazardPosition position;
    u8 unkEC[0x16];
    u8 state;
    u8 unk103[0x15];
};
struct GrowthHazardManager {
    u8 unk0[0x18];
    u8 count;
    u8 eventFlags;
    u8 unk1A[0x42];
    struct GrowthHazardEntry entries[1];
};

s32 sub_081B7568(void *object, struct GrowthHazardPosition *position,
                 struct GrowthHazardPosition *out)
{
    struct GrowthHazardManager *collection = object;
    struct GrowthHazardEntry *entry = collection->entries;
    s32 bestDistance = 0x7FFFFFFF;
    struct GrowthHazardPosition *best = NULL;
    s32 bestIndex = -1;
    s32 i;
    for (i = 0; i < collection->count; i++) {
        if (entry->state == 2) {
            struct GrowthHazardPosition *point = &entry->position;
            s32 x = position->x - point->x;
            s32 z = position->z - point->z;
            s32 distance = x * x + z * z;
            if (distance < bestDistance) {
                bestDistance = distance;
                best = point;
                bestIndex = i;
            }
        }
        entry++;
    }
    if (best == NULL)
        return -1;
    if (out != NULL)
        *out = *best;
    return bestIndex;
}
