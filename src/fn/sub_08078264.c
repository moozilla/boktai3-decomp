#include "scene_actor_behavior.h"
void SceneActorBehavior_CallActionScript(struct SceneActorBehaviorView *actor)
{
    u32 script = *(u16 *)&actor->script1F0;
    if (script)
        Script_Call(script, NULL);
}
