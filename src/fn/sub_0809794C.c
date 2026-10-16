#include "global.h"

u32 SceneActorProfiles_FindOrAdd(u32, u32);

void sub_0809794C(u8 *p)
{
    *(u32 *)(p + 0x308) = SceneActorProfiles_FindOrAdd(0x17, 0);
}
