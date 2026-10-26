#include "global.h"
#include "engine.h"
extern u32 gUnk_030053F0;
void SceneMessages_RegisterReceiver(u8 *, u32, u32);
void RegistryA8CB_RegisterMessageReceiver(u8 *p)
{
    u32 m = 0x800;
    if (((gEngineFlags | gUnk_030053F0) & m) == 0)
        SceneMessages_RegisterReceiver(p + 0x858, *(u32 *)(p + 0x8bc), 0xc);
}
