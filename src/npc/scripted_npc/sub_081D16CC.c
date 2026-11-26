#include "global.h"

void Task_Kill(u32);

void SceneNpc_DestroyTransitionEffect(u32 a)
{
    Task_Kill(a);
}
