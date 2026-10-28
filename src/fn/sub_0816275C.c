#include "global.h"
#include "ram.h"

void PlayerAttackEffect_SetDefaultPaletteSource(u32 v)
{
    if (gPlayerAttackEffect != 0) {
        if (*(u32 *)(gPlayerAttackEffect + 0x5A0) == 0)
            *(u32 *)(gPlayerAttackEffect + 0x24) = v;
    }
}
