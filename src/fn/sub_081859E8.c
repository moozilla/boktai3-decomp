#include "global.h"
struct E {
    u8 f0[0xB3D];
    u8 a;
    u8 f1[7];
    u8 b;
};
s32 DirectionalActor_TryBeginState(struct E *, s32);
void Armor_StartAction14(struct E *p)
{
    if (p->a == 0) {
        if (DirectionalActor_TryBeginState(p, 0xe) != 0)
            p->a = 0xff;
    }
}
