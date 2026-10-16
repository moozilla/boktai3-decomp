#include "global.h"

u32 SceneActorProfiles_FindOrAdd(u32, u32);

void sub_0808F9E8(u8 *p)
{
    *(u32 *)(p + 0x308) = SceneActorProfiles_FindOrAdd(0x16, 0);
}
