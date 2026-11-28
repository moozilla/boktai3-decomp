#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
#include "scene/actor/scene_actors.h"

// confidence=low: Equal distances favor the later point; the excluded index is never selected.
s32 Ratatosk_FindPointNearestReferenceExcept(void *unused, s32 excluded)
{
    s32 selected = -1;
    u32 minimum = -1;
    struct Ratatosk *self = (struct Ratatosk *)SceneActors_GetBySlot(4);
    s32 i;

    if (!self)
        return selected;
    for (i = 0; i < 9; i++) {
        if (i != excluded && self->orbPoints[i].anchorDistance <= minimum) {
            minimum = self->orbPoints[i].anchorDistance;
            selected = i;
        }
    }
    return selected;
}
