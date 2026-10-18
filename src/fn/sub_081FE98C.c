#include "global.h"
struct S {
    u8 p[4];
    u16 h4;
    u8 p6[2];
    u32 w8;
};
u32 Ratatosk_GetPosition(void);
u32 Player_GetEffectivePosition(u32);
void SunOrb_SetTargetSource(struct S *p, u32 n)
{
    p->h4 = n;
    if (n == 1)
        p->w8 = Ratatosk_GetPosition();
    else if (n == 2)
        p->w8 = Player_GetEffectivePosition(0);
}
