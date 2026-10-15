#include "scene_steering_actor.h"
static inline u8 Tst(u32 *m, s32 i)
{
    if (*m & (1 << i))
        return TRUE;
    return FALSE;
}

struct SceneSteeringActor *SceneSteeringActor_Allocate(struct SceneSteeringActorPool *b)
{
    s32 i;
    struct SceneSteeringActor *e;
    for (i = 0; i < 8; i++) {
        if (!Tst(&b->used, i)) {
            e = &b->slots[i];
            b->used |= 1 << i;
            e->index = i;
            return e;
        }
    }
    return 0;
}
