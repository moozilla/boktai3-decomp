#include "global.h"
struct S {
    u8 f0[0xB3D];
    u8 b3d;
    u8 f1[2];
    u8 b40;
};
void ActorController_ForceAction(struct S *, s32);
s32 sub_08184FE8(struct S *, s32);
void Armor_FinalizeDefeat(struct S *);
void Armor_UpdateDeathSequence(struct S *p)
{
    switch (p->b3d) {
        case 0:
            ActorController_ForceAction(p, 4);
            p->b3d = 1;
            break;
        case 1:
            if (p->b40 == 0xff) {
                sub_08184FE8(p, 0x12);
                p->b3d = 2;
            }
            break;
        case 2:
            if (p->b40 == 0xff) {
                p->b3d = 0xff;
                Armor_FinalizeDefeat(p);
            }
            break;
    }
}
