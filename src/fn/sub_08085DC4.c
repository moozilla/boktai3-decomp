#include "global.h"

void SceneActorReaction_HandleColliderContact(void);

void Ghoul_OnBodyCollision(void)
{
    SceneActorReaction_HandleColliderContact();
}
