#include "global.h"

void SceneMessages_UnregisterReceiver(u8 *);

u32 SceneCommands_TaskDestroy(u8 *p)
{
    SceneMessages_UnregisterReceiver(p + 0x18);
    return 0;
}
