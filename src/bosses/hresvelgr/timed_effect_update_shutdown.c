#include "engine/shared/global.h"
struct TimedEffect {
    u8 unknown0[0x18];
    u32 flags;
    u8 unknown1C[0xC];
    u16 state;
    u8 unknown2A[0x36];
    struct {
        u16 unknown0[3];
        u16 flags;
    } animation;
    u8 unknown68[0x54];
    u32 clearedBC[2];
    u8 unknownC4[0x76];
    u16 timer;
};
void Task_Kill(void *);
static inline void ClearFlags(u16 *p, u32 mask)
{
    p[3] &= ~mask;
}

void TimedEffect_UpdateShutdown(struct TimedEffect *p)
{
    if (p->timer == 0) {
        p->state = 6;
        ClearFlags((u16 *)&p->animation, 4);
        ZeroFill8(p->clearedBC);
        p->timer++;
    }
    if (p->timer > 0x7f)
        Task_Kill(p);
    else if (p->timer == 0x60)
        p->flags |= 0x200;
    p->timer++;
}
