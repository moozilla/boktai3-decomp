#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
#include "scene/actor/scene_actors.h"

static inline void ContactPosition_Copy(struct ContactPosition *d, const struct ContactPosition *s)
{
    *d = *s;
}

s32 sub_081F348C(s32 index, struct ContactPosition *out)
{
    struct Ratatosk *self = (struct Ratatosk *)SceneActors_GetBySlot(4);

    if (!self)
        return -1;
    ContactPosition_Copy(out, &self->orbPoints[index].position);
    return 0;
}
