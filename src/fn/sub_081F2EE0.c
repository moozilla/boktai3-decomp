#define SceneDistance_UpdateLinkedTargetDistances sub_081F2EE0
#include "global.h"
#include "scene_vec.h"
struct SceneDistanceEntry {
    struct SceneVec position;
    u8 pad8[4];
    s32 distanceSquared;
    s32 targetDistanceSquared;
    u8 pad14[4];
};
struct SceneDistanceSet {
    u8 pad0[0x36C];
    struct SceneVec position;
    u8 pad374[0xA7C];
    struct SceneDistanceEntry linked[4];
    u8 padE50[0xDC];
    u8 *source;
};
u8 *SunOrb_GetPositionFromManager(u8 *, s32);
struct SceneVec *Player_GetEffectivePosition(s32);
void sub_081F2EE0(struct SceneDistanceSet *set)
{
    s32 i = 0;
    s32 j = 0;
    do {
        struct SceneVec *position = Player_GetEffectivePosition(0);
        struct SceneVec *target = (struct SceneVec *)SunOrb_GetPositionFromManager(set->source, i);
        s32 x = target->x - position->x;
        s32 z = target->z - position->z;
        x *= x;
        x += z * z;
        set->linked[j].targetDistanceSquared = x;
        j++;
        i++;
    } while (i <= 3);
}
