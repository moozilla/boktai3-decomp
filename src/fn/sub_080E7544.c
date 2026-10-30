#include "global.h"
#include "scene_actor_reset.h"
u32 SceneActor_GetDirectionFlags(u32);
void SceneActorAnimation_SelectEntryFlags(void *, u32, u32);
static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}
void SceneActor_RestoreSavedTargetOnCompletion(struct SceneActorRegistryResetView *p)
{
    if (TakeFlag(&p->field2B2)) {
        u32 flags = SceneActor_GetDirectionFlags(p->body.direction);
        struct ContactCollider *collider;
        SceneActorAnimation_SelectEntryFlags(p, 0, flags);
        collider = &p->colliderA;
        collider->flags = 4 | collider->flags;
    }
    {
        u32 mask = 0x10;
        if (p->flags2D0 & mask) {
            u32 zero;
            void *saved;
            struct ContactCollider *collider = &p->colliderA;
            collider->flags &= 0xFFFB;
            zero = 0;
            saved = p->savedTarget;
            p->field2B2 = 1;
            p->field2B0 = zero;
            p->field2B5 = zero;
            p->target = saved;
            p->field2C8 = zero;
            {
                u32 *flags = &p->flags114;
                u32 clear = ~1;
                *flags &= clear;
            }
            p->word104 = zero;
            p->state2A8 = zero;
            p->phase2A9 = zero;
            p->behavior2AA = zero;
            p->counter2AC = zero;
            p->field2B1 = 1;
        }
    }
}
