#include "scene/actor/scene_actor_drawable_flags.h"
#include "scene/motion/scene_actor_chase_shared.h"
#include "npc/manager/scene_actor_animation.h"
#include "engine/video/anim_sprite.h"
#include "global.h"
#include "scene/actor/scene_actor_placement.h"
static inline void ClearCollider(struct ContactCollider *p)
{
    p->flags = 0xFFFB & p->flags;
}
void ActorShadow_Enable(void *);
void SceneActorPlacement_ShowActor(struct RoomActor *s)
{
    if (!SceneActorPlacement_IsUnavailable(s)) {
        u32 zero = 1;
        u32 *field = &s->behaviorFlags;
        u32 flags = *field;
        zero &= flags;
        if (!zero && (flags & 2)) {
            *field = flags & ~2;
            if (s->shadow[0])
                ActorShadow_Enable(s->shadow);
            ClearCollider(&s->colliderA);
            s->colliderA.responseValue = zero;
            ActorStatus_ClearDrawableFlags((void *)s, 1);
        }
    }
}
