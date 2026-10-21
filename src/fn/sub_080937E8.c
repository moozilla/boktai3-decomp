#include "global.h"

void Bee_UpdateFlightHeight(void);

u32 Bee_UpdateGroundHeight(void)
{
    Bee_UpdateFlightHeight();
    return 1;
}
