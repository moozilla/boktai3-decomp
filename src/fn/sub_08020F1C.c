#include "global.h"
#include "scene_event_dispatch.h"
void SceneMessages_RequestAdvance(void);
int SceneMessages_ConsumeSignal(void)
{
    u8 *p = gSceneEventDispatchState;
    if (!p || !*(int *)(p + 0x70))
        return -1;
    *(int *)(p + 0x70) = 0;
    SceneMessages_RequestAdvance();
    return 0;
}
