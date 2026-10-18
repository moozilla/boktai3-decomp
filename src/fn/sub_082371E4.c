// COMPILER: old_agbcc
#include "global.h"
/* RisingPuffPool_ClearInstance: Clears the singleton pointer for the eight-slot rising effect pool; puff identity is tentative */
#define RisingPuffPool_ClearInstance sub_082371E4

extern u32 gUnk_020004A0;

void sub_082371E4(void)
{
    gUnk_020004A0 = 0;
}
