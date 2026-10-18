// COMPILER: old_agbcc
#include "global.h"
/* Blend_Disable: Disables blending and restores default alpha and brightness register values */
#define Blend_Disable sub_082195AC

void sub_082195AC(void)
{
    *(vu16 *)0x04000050 = 0;
    *(vu16 *)0x04000052 = 0x1000;
    *(vu16 *)0x04000054 = 0;
}
