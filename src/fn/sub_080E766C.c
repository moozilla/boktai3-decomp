#define SceneActor_WaitForCategoryAttachment sub_080E766C
#define SceneActor_GetDirectionFlags         sub_080E81CC
#include "global.h"
#include "scene_actor_reset.h"
u32 sub_080E81CC(u32);
void SceneActorAnimation_SelectEntryFlags(void *, u32, u32);
void sub_0807C72C(void *, u32, u32, u32);
s32 ActorAttachment_IsUnavailable(void *);
static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}
void sub_080E766C(struct SceneActorRegistryResetView *p, s32 frame)
{
    if (TakeFlag(&p->field2B2)) {
        u32 flags = sub_080E81CC(p->direction);
        struct ContactCollider *collider;
        SceneActorAnimation_SelectEntryFlags(p, 0, flags);
        collider = &p->colliderB;
        collider->flags = 4 | collider->flags;
    }
    if (frame == 160) {
        sub_0807C72C(p, 0, 2, p->category);
    } else if (frame > 160) {
        if ((u8)ActorAttachment_IsUnavailable(p->attachment))
            p->field2B0 = 1;
    }
}
