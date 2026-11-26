#include "scene/actor/scene_actor_drawable_flags.h"
#include "npc/manager/scene_actor_animation.h"
#include "engine/video/anim_sprite.h"
#include "global.h"
#include "scene/actor/scene_actor_placement.h"
#include "scene/motion/scene_actor_chase_shared.h"
void ActorShadow_Disable(void *);
static inline void MarkCollider(struct ContactCollider *p)
{
    p->flags = 4 | p->flags;
}
void SceneActorPlacement_HideActor(struct RoomActor *s)
{
    if (!SceneActorPlacement_IsUnavailable(s)) {
        u32 bit = 2;
        u32 flags = s->behaviorFlags;
        if (!(flags & bit)) {
            s->behaviorFlags = flags | bit;
            ActorStatus_ClearDrawableFlags((void *)s, 2);
            ActorStatus_SetDrawableFlags((void *)s, 1);
            MarkCollider(&s->colliderA);
            if (s->shadow[0])
                ActorShadow_Disable(s->shadow);
        }
    }
}
