#include "global.h"
#include "scene_event_dispatch.h"

struct S {
    u8 f[0x1e];
    u8 a;
};
s32 SceneMessages_RequestAdvance(void)
{
    struct S *s = gSceneEventDispatchState;
    if (s == 0)
        return -1;
    if (s->a != 0)
        return -2;
    s->a = 1;
    return 0;
}
