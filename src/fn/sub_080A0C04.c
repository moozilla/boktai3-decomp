#include "global.h"

void Crow_UpdateGroundHeight(void);

u32 Crow_UpdatePreAction(void)
{
    Crow_UpdateGroundHeight();
    return 1;
}
