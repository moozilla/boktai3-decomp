#include "global.h"
void SceneLink_RemoveNode(void *);
void SceneMessages_UnregisterReceiver(void *);
void SceneBody_Unregister(void *);
void SceneActors_Unregister(int);
int SceneActors_Destroy(u8 *p)
{
    SceneLink_RemoveNode(p + 0x15c);
    SceneMessages_UnregisterReceiver(p + 0x118);
    SceneBody_Unregister(p + 0x48);
    SceneActors_Unregister(p[0x9b]);
    return 0;
}
