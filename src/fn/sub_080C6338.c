#include "scene_actor_chase.h"
void Golem_UpdateChargeBuildup(void);

#define Golem_BeginChargeBuildup sub_080C6338
void Golem_BeginChargeBuildup(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry(actor, 0xE);
        {
            u32 mask = 0x100;
            actor->flags2D4 = mask | actor->flags2D4;
        }
    }
    if (actor->field2B3)
        Chase_SetCtl(actor, Golem_UpdateChargeBuildup, 0x22);
}
