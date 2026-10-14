#include "tether_actor.h"
extern u8 *gUnk_02000488;
void TetherActor_SetState(struct TetherActor *, u32);
u32 sub_08066DB8(u32, u8 *, u32, u32, u32, u32, u32, u32);
void TetherActor_UpdateChargeDecay(struct TetherActor *p)
{
    u16 *q = &p->charge;
    u32 z;
    if (*q != 0) {
        u32 v = *q - 1;
        z = 0;
        *q = v;
        if ((u16)v == 1) {
            p->chargeReady = z;
            *q = z;
            *(u32 *)((u8 *)p + 0x47C) = sub_08066DB8(
                *(u32 *)((u8 *)p + 0x47C), (u8 *)&p->bodyConfig, 0x7F, z, z, z, 0xFA, 0x122);
        }
    }
}
