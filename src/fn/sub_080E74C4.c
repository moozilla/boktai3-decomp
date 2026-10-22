#include "global.h"
#include "scene_actor_animation.h"
u32 SceneActor_GetDirectionFlags(u32);
u8 SceneActor_IsCollisionTargetUnavailable(struct SceneActorAnimationView *);
void ActorShadow_Disable(void *);
static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}
void SceneActor_EnterShadowlessPhase(struct SceneActorAnimationView *p)
{
    if (TakeFlag(&p->directionalEntryFlag)) {
        if (SceneActor_IsCollisionTargetUnavailable(p)) {
            u32 flags = SceneActor_GetDirectionFlags(p->direction);
            SceneActorAnimation_SelectEntryFlags(p, 15, flags);
        } else {
            u32 flags = SceneActor_GetDirectionFlags(p->direction);
            SceneActorAnimation_SelectEntryFlags(p, 16, flags);
        }
        ActorShadow_Disable((u8 *)p + 0x320);
    }
    if (p->restart)
        p->unknown2B0 = 1;
}
