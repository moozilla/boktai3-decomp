// COMPILER: old_agbcc
#include "global.h"
#include "ram.h"
/* ScaledShadowTask_Destroy: Resets gScaledShadowManager to zero; structurally mirrors ShadowTask_Destroy and likely serves as the analogous teardown callback for the following ScaledSha */
#define ScaledShadowTask_Destroy sub_080136A8

void sub_080136A8(void)
{
    gScaledShadowManager = 0;
}
