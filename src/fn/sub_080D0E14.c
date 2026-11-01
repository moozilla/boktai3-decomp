#include "global.h"

void SceneActorReaction_HandleColliderContact(void);

void Mummy_OnBodyCollision(void)
{
    SceneActorReaction_HandleColliderContact();
}
