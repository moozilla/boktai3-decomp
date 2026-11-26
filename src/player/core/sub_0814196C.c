#include "global.h"

void SceneActorRegistry_SendToCategory(s32, u16 *);

void Player_NotifyActionCategories(void)
{
    u16 buf;

    buf = 6;
    SceneActorRegistry_SendToCategory(0x11, &buf);
    SceneActorRegistry_SendToCategory(0x17, &buf);
}
