#include "global.h"
#include "scene_actor_control_helpers.h"

struct SceneActorTargetLink {
    u8 *target;
    u8 unknownE0[0xB];
    u8 flags;
};

void SceneActorControl_ResetTargetFlags(struct ChaseActor *input)
{
    struct ChaseActor *s = input;
    struct SceneActorPendingTargetState *q = s->base.context;
    u16 *hp = &s->base.motionFlags;
    u32 hm = 0xFFFFFEFF;
    struct SceneActorTargetLink *t;
    u32 bm;
    u16 zero;
    u16 hv;
    hv = *hp & hm;
    zero = 0;
    *hp = hv;
    t = (void *)((u8 *)s + 0xDC);
    bm = ~8;
    t->flags = t->flags & bm;
    q->field444 = zero;
}
