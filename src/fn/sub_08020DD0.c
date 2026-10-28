#include "global.h"
#include "scene_event_dispatch.h"
s32 SceneMessages_Enqueue(s32, s32);
s32 SceneMessages_Send(s32 a)
{
    s32 r;
    if (gSceneEventDispatchState)
        r = SceneMessages_Enqueue(gSceneEventDispatchState, a);
    else
        r = -1;
    return r;
}
