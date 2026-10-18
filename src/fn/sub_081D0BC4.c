#include "global.h"

void SceneNpc_UpdateMode(void *);

s32 SceneNpc_OnModeUpdate(void *p)
{
    SceneNpc_UpdateMode(p);
    return 0;
}
