#include "global.h"

void SceneMessages_UnregisterReceiver(u8 *);

void Player_UnregisterSceneMessageReceiver(u8 *p)
{
    SceneMessages_UnregisterReceiver(p + 0x2F8);
}
