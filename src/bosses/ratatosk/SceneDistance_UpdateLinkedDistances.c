#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
#include "engine/math/scene_vec.h"
#include "objects/sun_orb/sun_orb_effect.h"

void SceneDistance_UpdateLinkedDistances(struct Ratatosk *self)
{
    s32 i = 0;
    const struct ContactPosition *anchor = &self->anchor;

    for (; i < 4; i++) {
        struct OrbitEffectPosition *orb = SunOrb_GetPositionFromManager(self->sunOrbs, i);
        s32 x = orb->x - anchor->x;
        s32 z = orb->z - anchor->z;

        s32 distance = x * x + z * z;

        self->targets[i].anchorDistance = distance;
    }
}
