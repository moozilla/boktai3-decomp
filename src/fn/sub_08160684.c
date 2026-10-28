#include "global.h"
#include "ram.h"

void PlayerAttackEffect_SetEmissionState(u32 v)
{
    if (gPlayerAttackEffect != 0) {
        if (*(u32 *)(gPlayerAttackEffect + 0x598) != v)
            *(u32 *)(gPlayerAttackEffect + 0x584) = 0;
        *(u32 *)(gPlayerAttackEffect + 0x59C) = v;
    }
}
