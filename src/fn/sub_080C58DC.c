#include "global.h"
void SceneActorReaction_HandleColliderContact(void);
void Golem_OnBodyCollision(void)
{
    SceneActorReaction_HandleColliderContact();
}
