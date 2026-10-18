#include "global.h"

void Mummy_ClearInactiveActionFlags(void);

s32 Mummy_UpdateSpecialFlags(void)
{
    Mummy_ClearInactiveActionFlags();
    return 1;
}
