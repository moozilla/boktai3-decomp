#include "scene_actor_reset.h"
void SceneActorTarget_ApplyResetCommand(struct SceneActorRegistryResetView *, const u16 *);
void SceneActorScript_ApplyTargetResetCommand(struct SceneActorRegistryResetView *actor,
                                              const u8 *script)
{
    SceneActorTarget_ApplyResetCommand(actor, (const u16 *)(script + 2));
}
