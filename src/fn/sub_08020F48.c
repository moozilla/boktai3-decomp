#include "global.h"
#include "scene_event_dispatch.h"
/* SceneMessages_IsActive: Returns whether the global controller exists and its playback state equals one. */
#define SceneMessages_IsActive sub_08020F48
#define SceneMessages_IsActive sub_08020F48
int SceneMessages_IsActive(void)
{
    u8 *p = gSceneEventDispatchState;
    int result;
    if (!p || p[0x23] != 1)
        result = 0;
    else
        result = 1;
    return result;
}
