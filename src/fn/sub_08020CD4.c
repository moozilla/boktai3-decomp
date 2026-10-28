#include "global.h"
#include "scene_event_dispatch.h"
int SceneMessages_LinkReceiver(void *, void *);
int SceneMessages_RegisterReceiver(u8 *p, int id, int kind)
{
    int result;
    if (gSceneEventDispatchState) {
        int zero = 0;
        *(u16 *)p = id;
        p[2] = kind;
        p[6] = zero;
        p[7] = zero;
        p[3] = zero;
        result = SceneMessages_LinkReceiver(gSceneEventDispatchState, p);
    } else
        result = -1;
    return result;
}
