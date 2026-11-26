#include "global.h"

void SceneNpc_UpdateMode(void *, u32);

s32 SceneNpc_OnModeUpdate(void *p, u32 mode)
{
    SceneNpc_UpdateMode(p, mode);
    return 0;
}
