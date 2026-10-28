#include "global.h"
#include "scene_event_dispatch.h"

struct S {
    u8 f[0x78];
    u8 a;
};
void SceneMessages_UnregisterReceiver(void *);
u32 SceneMessages_Destroy(struct S *p)
{
    SceneMessages_UnregisterReceiver(&p->a);
    return gSceneEventDispatchState = 0;
}
