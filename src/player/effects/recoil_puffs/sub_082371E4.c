// COMPILER: old_agbcc
#include "global.h"
#include "engine/ram.h"
/* RecoilPuffPool_ClearInstance: Clears the singleton pointer for the eight-slot rising effect pool; puff identity is tentative */

void RecoilPuffPool_ClearInstance(void)
{
    gRisingSpritePool = 0;
}
