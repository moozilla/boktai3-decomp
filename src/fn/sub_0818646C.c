#include "global.h"
struct E {
    u8 f0[0xB3D];
    u8 a;
    u8 f1[7];
    u8 b;
};
void DirectionalActor_TryBeginState(struct E *, s32);
void Armor_StopAndFinishBehavior(struct E *p)
{
    DirectionalActor_TryBeginState(p, 0);
    p->a = 0xff;
}
