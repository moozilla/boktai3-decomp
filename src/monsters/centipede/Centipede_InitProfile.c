#include "monsters/centipede/centipede_body.h"
void Centipede_InitProfile(struct RoomActor *actor)
{
    actor->profileId = SceneActorProfiles_FindOrAdd(actor->category, 0);
}
