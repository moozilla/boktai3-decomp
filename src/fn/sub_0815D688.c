#include "global.h"

struct P {
    u8 f00[0x24];
    u32 w24;
    u8 f28[0x2F8 - 0x28];
    u8 sub[1];
};

void SceneMessages_RegisterReceiver(u8 *, u32, s32);

void sub_0815D688(struct P *p)
{
    SceneMessages_RegisterReceiver(p->sub, p->w24, 2);
}
