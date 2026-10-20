#include "global.h"
#include "scene_actor_animation.h"
/* SceneActor_EnterFlaggedEntryPhase: plays directional entry 16 once, clears the 0x4 bit, and forwards the restart flag */
#define SceneActor_EnterFlaggedEntryPhase sub_080E70CC
u32 SceneActor_GetDirectionFlags(u32);
static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}
void SceneActor_EnterFlaggedEntryPhase(struct SceneActorAnimationView *p)
{
    if (TakeFlag(&p->directionalEntryFlag)) {
        u32 flags = SceneActor_GetDirectionFlags(p->direction);
        u16 *h;
        SceneActorAnimation_SelectEntryFlags(p, 16, flags);
        h = (u16 *)((u8 *)p + 0x188);
        h[3] &= 0xFFFB;
    }
    if (p->restart)
        p->unknown2B0 = 1;
}
