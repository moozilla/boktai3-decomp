#include "global.h"

struct PointDistance {
    u32 referenceDistance;
    u8 unk_04[20];
};
struct RatatoskPointSet {
    u8 unk_00[0xD24];
    struct PointDistance points[9];
};
struct RatatoskPointSet *SceneActors_GetBySlot(u32);
// confidence=low: the cached primary distance is tentatively associated with a reference.
// Equal distances favor the later point; the excluded index is never selected.
s32 Ratatosk_FindPointNearestReferenceExcept(void *unused, s32 excluded)
{
    s32 best = -1;
    u32 minimum = best;
    struct RatatoskPointSet *actor = SceneActors_GetBySlot(4);
    if (!actor)
        return best;
    {
        s32 index = 0;
        struct PointDistance *point = actor->points;
        do {
            if (index != excluded) {
                u32 distance = point->referenceDistance;
                if (distance <= minimum) {
                    minimum = distance;
                    best = index;
                }
            }
            point++;
            index++;
        } while (index <= 8);
    }
    return best;
}
