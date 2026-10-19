#include "global.h"
#include "scene_actor_reset.h"
#ifdef NONMATCHING
// fuzzy: 92.6%
// foundry draft (worker best 92.6%); what differs is mostly register allocation and instruction order
#define SceneActor_ClearReactionFlags sub_08135F38

static inline u32 ClearReaction(u16 *flags, u32 mask)
{
    return mask & *flags;
}
void sub_08135F38(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2DC & 0xC4000) {
        actor->flags2D6 = ClearReaction(&actor->flags2D6, ~0x9C9);
        actor->field2B8 = 0;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08135F38);
#endif
