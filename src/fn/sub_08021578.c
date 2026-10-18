#include "global.h"
void sub_0815F738(void *);
void SceneMessages_UnregisterReceiver(void *);
void SceneBody_Unregister(void *);
void SceneActors_Unregister(int);
int SceneActors_Destroy(u8 *p)
{
    sub_0815F738(p + 0x15c);
    SceneMessages_UnregisterReceiver(p + 0x118);
    SceneBody_Unregister(p + 0x48);
    SceneActors_Unregister(p[0x9b]);
    return 0;
}
