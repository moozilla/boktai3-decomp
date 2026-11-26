#include "global.h"
bool32 PlayerLink_IsActionModeAllowed(void);
s32 sub_0823A53C(void)
{
    if (PlayerLink_IsActionModeAllowed() != FALSE)
        return 10;
    return 0;
}
