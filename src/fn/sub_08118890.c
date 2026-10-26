#include "global.h"
#include "engine.h"
extern u32 gUnk_030053F0;
void SceneMessages_RegisterReceiver(void *, u32, u32);
void Glove_RegisterMessageReceiver(u8 *s)
{
    u32 m = 0x800;
    if (((gEngineFlags | gUnk_030053F0) & m) == 0) {
        SceneMessages_RegisterReceiver(s + 0x870, *(u32 *)(s + 0x910), 0xb);
    }
}
