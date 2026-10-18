// COMPILER: old_agbcc
#include "global.h"
/* ScaledShadowTask_Destroy: Resets gUnk_02000058 to zero; structurally mirrors ShadowTask_Destroy and likely serves as the analogous teardown callback for the following ScaledSha */
#define ScaledShadowTask_Destroy sub_080136A8

extern u32 gUnk_02000058;

void sub_080136A8(void)
{
    gUnk_02000058 = 0;
}
