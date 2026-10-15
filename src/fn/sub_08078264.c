#include "scene_actor_behavior.h"
#define SceneActorBehavior_CallActionScript sub_08078264
void SceneActorBehavior_CallActionScript(struct SceneActorBehaviorView *actor)
{
    u32 script = *(u16 *)&actor->script1F0;
    if (script)
        Script_Call(script, NULL);
}
