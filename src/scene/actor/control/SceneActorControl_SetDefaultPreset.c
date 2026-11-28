#include "scene/actor/scene_actor_control_motion.h"
extern const u8 gUnk_08605C28[];
void SceneActorControl_SetDefaultPreset(struct ChaseActor *actor)
{
    actor->base.callbacks284 = (void **)gUnk_08605C28;
}
