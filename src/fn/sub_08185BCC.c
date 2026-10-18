#include "global.h"
struct E {
    u8 f0[0xB3D];
    u8 a;
    u8 f1[7];
    u8 b;
};
void ActorController_ForceAction(struct E *, s32);
void Armor_StartAction4(struct E *p)
{
    if (p->a == 0) {
        ActorController_ForceAction(p, 4);
        p->a = 1;
    }
}
