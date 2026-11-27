#include "global.h"
#include "engine/math/scene_vec.h"
#include "bosses/ratatosk/scene_distance_entry_layout.h"
#include "objects/sun_orb/sun_orb_effect.h"
#include "bosses/ratatosk/ratatosk_orb_distance_view.h"
struct SceneVec *Player_GetEffectivePosition(s32);
void SceneDistance_UpdateLinkedTargetDistances(struct RatatoskOrbDistanceView *set)
{
    s32 i = 0;
    s32 j = 0;
    do {
        struct SceneVec *position = Player_GetEffectivePosition(0);
        struct SceneVec *target = (struct SceneVec *)SunOrb_GetPositionFromManager(set->manager, i);
        s32 x = target->x - position->x;
        s32 z = target->z - position->z;
        x *= x;
        x += z * z;
        set->orbsStorage.linked[j].targetDistanceSquared = x;
        j++;
        i++;
    } while (i <= 3);
}
