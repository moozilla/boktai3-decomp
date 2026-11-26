#include "global.h"

void SceneActorRegistry_ClearObjectContext(void)
{
    *(u32 *)0x030025F8 = 0;
}
