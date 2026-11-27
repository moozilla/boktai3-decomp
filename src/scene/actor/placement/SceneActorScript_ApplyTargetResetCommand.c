#include "scene/actor/scene_actor_reset.h"
void SceneActorTarget_ApplyResetCommand(struct ChaseActor *, const u16 *);
void SceneActorScript_ApplyTargetResetCommand(struct ChaseActor *actor, const u8 *script)
{
    SceneActorTarget_ApplyResetCommand(actor, (const u16 *)(script + 2));
}
