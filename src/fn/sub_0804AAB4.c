#include "tether_actor.h"
u32 ActorAttachment_UpdateFromDescriptor(u32, u8 *, u32, u32, u32, u32, u32, u32);
void TetherActor_AccumulateCharge(struct TetherActor *p, u32 a)
{
    u16 *q = &p->charge;
    u32 v = *q + a;
    *q = v;
    if (p->chargeReady == 0) {
        if ((u16)v > 0x4B0) {
            *q = 0x4B0;
            if (p->interactionLock == 0) {
                if (p->state != 2) {
                    p->chargeReady = 1;
                    *(u32 *)((u8 *)p + 0x47C) = ActorAttachment_UpdateFromDescriptor(
                        *(u32 *)((u8 *)p + 0x47C), (u8 *)&p->bodyConfig, 1, 0, (u32)q, 0x4B0, 0xFA,
                        0x122);
                }
            }
        }
    } else {
        if ((u16)v > 0x4B0)
            *q = 0x4B0;
    }
}
