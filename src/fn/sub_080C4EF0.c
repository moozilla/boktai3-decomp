#include "global.h"
#include "scene_actor_chase.h"
#include "scene_actor_restoration.h"

extern const u8 gUnk_0824E274[];

void Golem_ResetIdleControl(struct SceneActorRegistryResetView *actor)
{
    SceneActorReaction_ResetResponseState(&actor->group.saved.control, gUnk_0824E274, 0x22F);
    {
        u32 phase = 2;
        actor->state2A8 = 0;
        actor->phase2A9 = phase;
        actor->behavior2AA = 0;
        actor->counter2AC = 0;
        actor->field2B1 = 1;
    }
    Chase_SetCtl(actor, actor->entryTarget228, 1);
}
