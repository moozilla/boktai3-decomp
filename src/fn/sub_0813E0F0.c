// COMPILER: old_agbcc
#include "global.h"
/* Player_GetEffectIntensity: Returns the global signed value also used to pace inward particle emission; intensity interpretation tentative */
#define Player_GetEffectIntensity sub_0813E0F0

extern u8 *gUnk_02000710;

s16 sub_0813E0F0(void)
{
    return *(s16 *)(gUnk_02000710 + 0x876);
}
