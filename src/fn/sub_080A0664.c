#include "global.h"

u32 SceneActorProfiles_FindOrAdd(u32, u32);

void sub_080A0664(u8 *p)
{
    *(u32 *)(p + 0x308) = SceneActorProfiles_FindOrAdd(0x13, 0);
}
