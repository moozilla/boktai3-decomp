#include "global.h"
#include "monsters/chase_actor.h"

u32 SceneActorProfiles_FindOrAdd(u32, u32);

void SceneActorControl_SetDefaultProfile(struct ChaseActor *actor)
{
    actor->base.profileId = SceneActorProfiles_FindOrAdd(0x12, 0);
}
