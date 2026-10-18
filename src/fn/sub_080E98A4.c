#include "global.h"

void SceneActorBehavior_ApplyContactReaction(void);

void Octopus_OnBodyContact(void)
{
    SceneActorBehavior_ApplyContactReaction();
}
