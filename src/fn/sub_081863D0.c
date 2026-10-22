#include "global.h"
struct E {
    u8 f0[0xB3D];
    u8 a;
    u8 f1[7];
    u8 b;
};
void ActorController_UpdateTargetMovement(struct E *, s32, s32);
void Armor_UpdateOffsetTracking(struct E *, s32, s32);
void Armor_UpdateDelayedTrackingAction31(struct E *p)
{
    if (p->b != 0) {
        p->b--;
        ActorController_UpdateTargetMovement(p, 0, 1);
    } else {
        Armor_UpdateOffsetTracking(p, 0, 1);
    }
}
