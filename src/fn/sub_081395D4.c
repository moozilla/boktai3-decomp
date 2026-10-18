#include "global.h"
void ParticleBurstEffect_SetStateCallback(u32 *p, u32 v)
{
    *p = v;
    *(u8 *)((u8 *)p + 0xcb) = 1;
}
