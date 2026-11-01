#include "global.h"

void SceneActorReaction_HandleColliderContact(void *, s32);

void Mimic_OnBodyContact(void *a, s32 b)
{
    SceneActorReaction_HandleColliderContact(a, b);
}
