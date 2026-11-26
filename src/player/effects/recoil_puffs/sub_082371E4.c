// COMPILER: old_agbcc
#include "global.h"
#include "engine/ram.h"
/* RecoilPuffPool_ClearInstance: Clears the singleton pointer for the eight-slot rising effect pool; puff identity is tentative */
#define RecoilPuffPool_ClearInstance sub_082371E4

void sub_082371E4(void)
{
    gRisingSpritePool = 0;
}
