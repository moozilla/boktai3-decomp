#include "global.h"
#include "ram.h"

void PlayerAttackEffect_SetElementMode(u32 v)
{
    if (gPlayerAttackEffect != 0)
        *(u32 *)(gPlayerAttackEffect + 0x5A0) = v;
}
