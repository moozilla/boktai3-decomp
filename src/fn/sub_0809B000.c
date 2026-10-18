#include "global.h"

void SceneActorBehavior_ApplyContactReaction(void);

void Lun_OnBodyContact(void)
{
    SceneActorBehavior_ApplyContactReaction();
}
