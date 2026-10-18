#include "global.h"
u32 SceneActorProfiles_FindOrAdd(u32, u32);
void TentacleActor_InitProfile(u8 *p)
{
    *(u32 *)(p + 0x308) = SceneActorProfiles_FindOrAdd(p[0x98], 0);
}
