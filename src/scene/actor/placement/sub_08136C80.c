#include "npc/manager/scene_actor_animation.h"
#include "ui/widgets/scene_input_widget.h"
#include "global.h"
#include "scene/actor/scene_actor_placement.h"
#include "scene/motion/scene_actor_chase_shared.h"
void ActorShadow_Disable(void *);
static inline void ClearTile(struct SceneInputWidget *p, u32 mask)
{
    p->flags &= mask;
}
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
            void *drawable;
            s->behaviorFlags = flags | bit;
            if (s->drawableKind == 0) {
                u32 *p = s->drawable;
                SceneActorChase_AndWordFlags(p, ~2);
                drawable = p;
            } else {
                struct SceneInputWidget *p;
                drawable = s->drawable;
                p = &((struct SceneActorAnimationTileDrawable *)drawable)->tile;
                ClearTile(p, ~2);
            }
            {
                u32 one = 1;
                if (s->drawableKind == 0) {
                    *(u32 *)drawable |= one;
                } else {
                    struct SceneInputWidget *p =
                        &((struct SceneActorAnimationTileDrawable *)drawable)->tile;
                    p->flags |= one;
                }
            }
            MarkCollider(&s->colliderA);
            if (s->shadow[0])
                ActorShadow_Disable(s->shadow);
        }
    }
}
