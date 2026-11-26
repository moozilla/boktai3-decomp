#include "global.h"

struct O {
    u8 f0[0x10];
    u16 h10;
    u8 f12[0xea];
    u16 hfc;
    u16 hfe;
    u8 f100[2];
    u8 b102;
    u8 f103[0x11];
    void (*cb)(void);
};
struct O2 {
    u8 f0[0xf4];
    u16 hf4;
};
void SolarRoot_UpdateCooldown(void);

void SolarRoot_BeginCooldown(struct O *p)
{
    u32 a = 2;
    void (*f)(void);
    p->h10 = a;
    p->hfc = p->hfe;
    f = SolarRoot_UpdateCooldown;
    p->cb = f;
    ((struct O2 *)p)->hf4 = 0;
    p->b102 = 2;
}
