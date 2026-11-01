#include "scene_actor_chase_reaction.h"
#include "scene_actor_delayed_response.h"
#include "scene_actor_reaction.h"

#include "scene_actor_contact_list.h"

#ifdef NONMATCHING
// fuzzy: 73.0%
// confidence=low: resume pursuit after releasing the actor's contact node.
void sub_0807DB34(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    struct ContactPosition *destination;

    if (actor->flags2D4 & 0x1000) {
        SceneActorContacts_ReleaseNode(group->contactNode);
        SceneActorChase_ClearBits(&actor->flags2D4, 0x1000);
        group->contactNode = 0;
    }
    actor->field2C2 = 64;
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
    SceneActorChase_SetState(actor, 3, 1, 0);
    if (actor->flags2D4 & 0x80) {
        if (SceneActorChase_Arrive(actor)) {
            SceneActorDelay_Prepare((void *)&group->saved, group->common.gate.control, 12, 60);
            group->common.gate.flags &= ~4;
        }
    } else {
        destination = group->destination;
        destination->pad = 1;
        if (!SceneActorChase_TryRoute(actor, group->route, &actor->body.position, destination)) {
            Callback_InvokeActor(actor, group->fallback);
            return;
        }
        SceneActorDelay_Prepare((void *)&group->saved, actor->entryTarget228, 7, 60);
        if (actor->flags2CC & 0x200000) {
            actor->flags2D0 &= ~0x01000000;
            actor->flags2D4 &= ~0x200;
            group->common.control.timer = 0;
            group->common.control.mode = 0;
            group->common.control.counter = 0;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0807DB34);
#endif
