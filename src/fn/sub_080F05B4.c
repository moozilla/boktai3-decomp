#include "global.h"
void SceneActorRegistry_SendToCategory(s32, s16 *);

void sub_080F05B4(void)
{
    s16 msg = 6;
    SceneActorRegistry_SendToCategory(0x18, &msg);
}
