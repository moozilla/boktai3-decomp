#include "global.h"

void Sprite_Remove(u8 *);
void SceneBody_Unregister(u8 *);
void sub_0815F738(u8 *);

s32 RoamingObjectB_Destroy(u8 *p)
{
    Sprite_Remove(p + 0x64);
    SceneBody_Unregister(p + 0x1c);
    sub_0815F738(p + 0x124);
    return 0;
}
