#define SceneActor_EnterShadowlessPhase sub_080E74C4
#define SceneActor_GetDirectionFlags    sub_080E81CC
#include "global.h"
#include "scene_actor_animation.h"
u32 sub_080E81CC(u32);
u8 sub_080E8444(struct SceneActorAnimationView *);
void ActorShadow_Disable(void *);
static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}
void sub_080E74C4(struct SceneActorAnimationView *p)
{
    if (TakeFlag(&p->directionalEntryFlag)) {
        if (sub_080E8444(p)) {
            u32 flags = sub_080E81CC(p->direction);
            SceneActorAnimation_SelectEntryFlags(p, 15, flags);
        } else {
            u32 flags = sub_080E81CC(p->direction);
            SceneActorAnimation_SelectEntryFlags(p, 16, flags);
        }
        ActorShadow_Disable((u8 *)p + 0x320);
    }
    if (p->restart)
        p->unknown2B0 = 1;
}
