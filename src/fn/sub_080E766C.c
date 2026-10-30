#include "global.h"
#include "scene_actor_reset.h"
u32 SceneActor_GetDirectionFlags(u32);
void SceneActorAnimation_SelectEntryFlags(void *, u32, u32);
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
s32 ActorAttachment_IsUnavailable(void *);
static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}
void SceneActor_WaitForCategoryAttachment(struct SceneActorRegistryResetView *p, s32 frame)
{
    if (TakeFlag(&p->field2B2)) {
        u32 flags = SceneActor_GetDirectionFlags(p->body.direction);
        struct ContactCollider *collider;
        SceneActorAnimation_SelectEntryFlags(p, 0, flags);
        collider = &p->colliderB;
        collider->flags = 4 | collider->flags;
    }
    if (frame == 160) {
        SceneActorAttachment_ReplaceResponseSlot(p, 0, 2, p->category);
    } else if (frame > 160) {
        if ((u8)ActorAttachment_IsUnavailable(p->group.attachment))
            p->field2B0 = 1;
    }
}
