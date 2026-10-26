// COMPILER: old_agbcc
#include "global.h"
#include "ram.h"
/* RisingPuffPool_ClearInstance: Clears the singleton pointer for the eight-slot rising effect pool; puff identity is tentative */
#define RisingPuffPool_ClearInstance sub_082371E4

void sub_082371E4(void)
{
    gRisingSpritePool = 0;
}
