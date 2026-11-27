#include "npc/story_character/scene_character.h"
s32 Actor_UpdateMovementAnimationAndEffects(void *, struct SceneNpc *);
void SceneNpc_DispatchCutsceneCommands(struct SceneNpc *);
s32 SceneNpc_UpdateCommands(void *context, struct SceneNpc *p)
{
    Actor_UpdateMovementAnimationAndEffects(context, p);
    SceneNpc_DispatchCutsceneCommands(p);
    return p->nextState = 0;
}
