#include "global.h"

void SceneMessages_RegisterReceiver(u8 *, s32, s32);

s32 sub_0806386C(u8 *p, s32 a)
{
    SceneMessages_RegisterReceiver(p + 0x1c, a, 8);
    return 0;
}
