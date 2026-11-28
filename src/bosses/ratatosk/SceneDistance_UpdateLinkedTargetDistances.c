#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
#include "engine/math/scene_vec.h"
#include "objects/sun_orb/sun_orb_effect.h"

struct SceneVec *Player_GetEffectivePosition(s32);

void SceneDistance_UpdateLinkedTargetDistances(struct Ratatosk *self)
{
    s32 i;

    for (i = 0; i < 4; i++) {
        struct SceneVec *player = Player_GetEffectivePosition(0);
        struct OrbitEffectPosition *orb = SunOrb_GetPositionFromManager(self->sunOrbs, i);
        s32 x = orb->x - player->x;
        s32 z = orb->z - player->z;

        s32 distance = x * x + z * z;

        self->targets[i].distance = distance;
    }
}
