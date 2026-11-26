#include "engine/shared/global.h"
#include "engine/shared/ram.h"

bool32 sub_08017428(void)
{
    if (gWeatherState != 0)
        return TRUE;
    return FALSE;
}
