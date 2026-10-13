#include "global.h"

void SceneMessages_RegisterReceiver(u8 *, u32, u32);

void sub_081CD46C(u8 *a, u32 b)
{
    u32 buf[4];
    SceneMessages_RegisterReceiver(a + 0x168, b, 5);
}
