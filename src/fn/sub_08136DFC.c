#include "scene_actor_animation.h"
#include "scene_input_widget.h"
#include "global.h"
#include "scene_actor_placement.h"
static inline void ClearWord(u32 *p, u32 mask)
{
    *p &= mask;
}
static inline void ClearTile(struct SceneInputWidget *p, u32 mask)
{
    p->flags &= mask;
}
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
            if (s->drawableKind == 0) {
                u32 *p = s->drawable;
                ClearWord(p, ~1);
            } else {
                struct SceneInputWidget *p =
                    &((struct SceneActorAnimationTileDrawable *)s->drawable)->tile;
                ClearTile(p, ~1);
            }
        }
    }
}
