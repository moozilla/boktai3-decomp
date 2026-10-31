#include "global.h"
struct PointDistance {
    u32 distance;
    u8 pad4[20];
};
void *SceneActors_GetBySlot(u32);
// confidence=low: chooses the smallest cached distance while excluding one point index.
#define Ratatosk_FindNearestPointExcept sub_081F3414
s32 Ratatosk_FindNearestPointExcept(void *unused, s32 excluded)
{
    s32 best = -1;
    u32 minimum = best;
    void *actor = SceneActors_GetBySlot(4);
    if (!actor)
        return best;
    {
        s32 index = 0;
        struct PointDistance *point = (void *)((u8 *)actor + 0xd28);
        do {
            if (index != excluded) {
                u32 distance = point->distance;
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
