#include "global.h"
#include "sun_orb_effect.h"

u8 *Ratatosk_GetPosition(void);
u8 *Player_GetEffectivePosition(u32);

void SunOrb_SetTargetSource(struct OrbitEffectParams *p, u32 n)
{
    p->kind = n;
    if (n == 1)
        p->targetSource = (struct OrbitEffectPosition *)Ratatosk_GetPosition();
    else if (n == 2)
        p->targetSource = (struct OrbitEffectPosition *)Player_GetEffectivePosition(0);
}
