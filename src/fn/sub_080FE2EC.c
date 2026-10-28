#include "global.h"
#include "scene_actor_reset.h"
#include "player.h"
void Axe_UpdateWaitingAnimation(void);
void Axe_BeginMoveToPosition(void *, void *);
static inline u8 TakeFlag(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
static inline u16 SetFlag(struct SceneActorRegistryResetView *actor, u32 mask)
{
    u16 *flags = &actor->flags2D4;
    return *flags = mask | *flags;
}
void Axe_UpdatePlayerDestinationPhase(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorRegistryControlGroup *group = &actor->group;
    if (TakeFlag(&actor->field2B1)) {
        void *callback = Axe_UpdateWaitingAnimation;
        u32 kind = 10;
        actor->field2B2 = 1;
        actor->field2B0 = 0;
        actor->field2B5 = kind;
        actor->target = callback;
        actor->field2C8 = 0;
        {
            u32 *flags = &actor->flags114;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->word104 = 0;
        SetFlag(actor, 0x100);
        group->field418 = group->field416;
    }
    if (actor->field2B0)
        Axe_BeginMoveToPosition(actor, ((u8 **)gPlayers)[0] + 0x30);
    else
        actor->counter2AC++;
}
