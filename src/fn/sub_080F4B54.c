#include "global.h"
#include "ram.h"

extern u8 gUnk_0203B400[];
struct P {
    u8 f[0x170];
    void (*cb)(void);
    u8 g[0x184 - 0x174];
    u32 a;
};
void ActorPhase_OrbitParent(void);

void Ivy_BeginChildRandomDelay(struct P *p)
{
    u32 v;
    gRandomIndex = (gRandomIndex + 1) & 0x3ff;
    v = *(u16 *)(gUnk_0203B400 + gRandomIndex * 2);
    p->a = (v & 0x40) + 0x1e;
    p->cb = ActorPhase_OrbitParent;
}
