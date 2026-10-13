#include "global.h"
void SceneMessages_RegisterReceiver(u8 *, u32, u32);
void sub_0805C73C(u8 *p)
{
    SceneMessages_RegisterReceiver(p + 0x17b0, *(u16 *)(p + 0x18), 9);
}
