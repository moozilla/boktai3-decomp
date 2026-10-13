#include "global.h"

extern u32 gUnk_02000560;
void SceneActors_RunStateCallback(u32);
void sub_0802AAD4(u32 a)
{
    u32 m = 4;
    if ((gUnk_02000560 & m) == 0)
        SceneActors_RunStateCallback(a);
}
